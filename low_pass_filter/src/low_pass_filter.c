
#include "low_pass_filter.h"

#include "window_functions.h"

// -----------------------------------------------------------------------------
// Struct containing data needed to low pass filter a buffer of samples.
// -----------------------------------------------------------------------------
typedef struct low_pass_filter
{
    float cutoff;
    int order;
    float* coeffs;
    float* past_input_samples;
    int write_point;
    enum window_t window_type;
    size_t buffer_size;
    int channel_count;
} low_pass_filter_t;

enum window_t init_filter(low_pass_filter_t* lpf,
                          float sample_rate,
                          enum window_t window_type);

void filter_buffer(low_pass_filter_t* lpf,
                   float* audio_buffer,
                   sf_count_t samples_read);

int get_read_point(low_pass_filter_t* lpf, int samples_delay);
void increment_write_point(low_pass_filter_t* lpf);

// -----------------------------------------------------------------------------
// Allocates memory for a low_pass_filter_t object and initialises its members.
// Allocation of coeffs and past_input_samples is left until filter is about to
// begin. Returns the address of the new object.
//
// Arguments:
//     cutoff      - -6dB point of filter
//     window_type - window to apply to filter coefficients
//     buffer_size - size of processing block
//
// Returns:
//     pointer to new low_pass_filter_t object
// -----------------------------------------------------------------------------
low_pass_filter_t* lpf_create(float cutoff, enum window_t window_type, size_t buffer_size)
{
    low_pass_filter_t* lpf =
        (low_pass_filter_t*)malloc(sizeof(low_pass_filter_t));
    if (lpf)
    {
        lpf->cutoff = cutoff;
        lpf->order = 126;
        lpf->coeffs = NULL;
        lpf->past_input_samples = NULL;
        lpf->write_point = 0;
        lpf->window_type = window_type;
        lpf->buffer_size = buffer_size;
        lpf->channel_count = 0;
    }

    return lpf;
}

// -----------------------------------------------------------------------------
// Opens input file and creates an output file to write to. Processes input
// data by block.
//
// Arguments:
//     lpf - pointer to low pass filter data
//     input_file_name - input file name
//     output_file_name - output file name
//     cutoff - -6dB point of filter
// -----------------------------------------------------------------------------
enum lpf_error lpf_filter_file(low_pass_filter_t* lpf,
                               const char* input_file_name,
                               const char* output_file_name,
                               enum window_t window_type,
                               sf_count_t* samples_filtered)
{
    SF_INFO wav_info;
    SNDFILE* input_wav = sf_open(input_file_name, SFM_READ, &wav_info);

    if (input_wav == NULL)
    {
        eprintf("unable to open input file\n");
        return LPF_FILE_OPEN_ERROR;
    }

    const sf_count_t samples_to_process = wav_info.frames;
    sf_count_t samples_processed = 0;

    float* audio_buffer = (float*)calloc(lpf->buffer_size, sizeof(float));

    if (init_filter(lpf, (float)wav_info.samplerate, window_type))
        return LPF_FILTER_INIT_ERROR;

    SNDFILE* output_wav = sf_open(output_file_name, SFM_WRITE, &wav_info);

    if (output_wav == NULL)
    {
        eprintf("unable to open output file\n");
        return LPF_FILE_OPEN_ERROR;
    }

    while (samples_processed < samples_to_process)
    {
        const sf_count_t samples_read =
            sf_read_float(input_wav, audio_buffer, lpf->buffer_size);

        filter_buffer(lpf, audio_buffer, samples_read);

        const sf_count_t samples_written =
            sf_write_float(output_wav, audio_buffer, samples_read);

        if (samples_written != samples_read)
        {
            eprintf("not all samples were written to the output file\n");
            return LPF_FILE_WRITE_ERROR;
        }

        samples_processed += samples_written;
    }

    *samples_filtered = samples_processed;

    sf_close(input_wav);
    sf_close(output_wav);

    free(lpf->coeffs);
    free(lpf->past_input_samples);
    free(audio_buffer);

    return LPF_NO_ERROR;
}

// -----------------------------------------------------------------------------
// Initialises coefficients.
//
// Arguments:
//     lpf - lpf - pointer to low pass filter data
//     sample_rate - sample rate
//     cutoff - -6dB point of filter
//     window_type - window to apply to filter coefficients
//
// Returns:
//     LPF_NO_ERROR on success
// -----------------------------------------------------------------------------
enum lpf_error init_filter(low_pass_filter_t* lpf,
                           float sample_rate,
                           enum window_t window_type)
{
    if (lpf->cutoff <= 0)
        return LPF_CUTOFF_ERROR;
    else if (sample_rate <= 0)
        return LPF_SAMPLE_RATE_ERROR;

    lpf->coeffs = (float*)calloc(((size_t)lpf->order + 1), sizeof(float));
    lpf->past_input_samples =
        (float*)calloc(((size_t)lpf->order + 1), sizeof(float));

    float transition_frequency = lpf->cutoff / sample_rate;

    for (int i = 0; i < lpf->order + 1; ++i)
    {
        if (i == lpf->order / 2.0f)
            lpf->coeffs[i] = 2.0f * transition_frequency;
        else
        {
            float pi_x = (float)M_PI * (i - lpf->order / 2.0f);
            lpf->coeffs[i] = sinf(2.0f * transition_frequency * pi_x) / pi_x;
        }
    }

    switch (window_type)
    {
    case BARTLETT: bartlett_window(lpf->coeffs, lpf->order + 1); break;
    case BLACKMAN: blackman_window(lpf->coeffs, lpf->order + 1); break;
    case HAMMING: hamming_window(lpf->coeffs, lpf->order + 1); break;
    case HANNING: hanning_window(lpf->coeffs, lpf->order + 1); break;
    case KAISER: kaiser_window(lpf->coeffs, lpf->order + 1); break;
    }

    // normalises coeffiecients to avoid clipping
    float sum = 0.0f;
    for (int i = 0; i < lpf->order + 1; ++i) sum += lpf->coeffs[i];
    for (int i = 0; i < lpf->order + 1; ++i) lpf->coeffs[i] /= sum;

    return LPF_NO_ERROR;
}

// -----------------------------------------------------------------------------
// Processes buffer.
//
// Arguments:
//     lpf          - pointer to low pass filter data
//     audio_buffer - buffer of samples
//     samples_read - length of buffer
//
// Returns:
//     void
// -----------------------------------------------------------------------------
void filter_buffer(low_pass_filter_t* lpf,
                   float* audio_buffer,
                   sf_count_t samples_read)
{
    for (int i = 0; i < samples_read; ++i)
    {
        lpf->past_input_samples[lpf->write_point] = audio_buffer[i];

        audio_buffer[i] = 0.0f;

        for (int j = 0; j < lpf->order + 1; ++j)
        {
            audio_buffer[i] += lpf->coeffs[j] *
                               lpf->past_input_samples[get_read_point(lpf, j)];
        }

        increment_write_point(lpf);
    }
}

// -----------------------------------------------------------------------------
// Gets read point of circular buffer based on a write point and a delay length.
//
// Arguments:
//     lpf           - used for write point and filter length
//     samples_delay - delay between 0 and filter length
//
// Returns:
//     read point
// -----------------------------------------------------------------------------
int get_read_point(low_pass_filter_t* lpf, int samples_delay)
{
    int read_point = lpf->write_point + lpf->order + 1 - samples_delay;
    if (read_point >= lpf->order + 1)
        read_point -= lpf->order + 1;

    return read_point;
}

// -----------------------------------------------------------------------------
// Increments write point in low_pass_filter_t object, and wraps it around
// filter length.
//
// Arguments:
//     lpf - owner of write_point to increment
//
// Returns:
//     void
// -----------------------------------------------------------------------------
void increment_write_point(low_pass_filter_t* lpf)
{
    ++lpf->write_point;
    if (lpf->write_point == lpf->order + 1)
        lpf->write_point = 0;
}

// -----------------------------------------------------------------------------
// Deallocates low_pass_filter_t object and its arrays.
//
// Arguments:
//      lpf - low_pass_filter_t to deallocate
//
// Returns:
//     void
// -----------------------------------------------------------------------------
void lpf_destroy(low_pass_filter_t* lpf)
{
    if (lpf)
    {
        free(lpf);
    }
}