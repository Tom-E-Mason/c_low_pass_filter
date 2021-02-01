
#include "low_pass_filter.h"

#include "window_functions.h"

typedef struct low_pass_filter
{
    int order;
    float* filter_coeffs;
    float* past_input_samples;
    int write_point;
    enum window_t window_type;
    size_t buffer_size;
} low_pass_filter_t;

enum window_t init_coeffs(low_pass_filter_t* lpf,
                          float sample_rate,
                          float cutoff,
                          enum window_t window_type);

void filter_buffer(low_pass_filter_t* lpf,
                   float* audio_buffer,
                   sf_count_t samples_read);

int get_read_point(low_pass_filter_t* lpf, int samples_delay);
void increment_write_point(low_pass_filter_t* lpf);

low_pass_filter_t* lpf_create(float cutoff,
                              enum window_t window_type,
                              size_t buffer_size)
{
    low_pass_filter_t* lpf =
        (low_pass_filter_t*)malloc(sizeof(low_pass_filter_t));
    if (lpf)
    {
        lpf->order = 126;
        lpf->filter_coeffs =
            (float*)calloc(((size_t)lpf->order + 1), sizeof(float));
        lpf->past_input_samples =
            (float*)calloc(((size_t)lpf->order + 1), sizeof(float));
        lpf->write_point = 0;
        lpf->window_type = window_type;
        lpf->buffer_size = buffer_size;
    }

    return lpf;
}

enum lpf_error lpf_filter_file(low_pass_filter_t* lpf,
                               const char* input_file_name,
                               const char* output_file_name,
                               float cutoff,
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

    sf_count_t samples_to_process = wav_info.frames;
    sf_count_t samples_processed = 0;

    float* audio_buffer = (float*)calloc(lpf->buffer_size, sizeof(float));

    if (init_coeffs(lpf, (float)wav_info.samplerate, cutoff, window_type))
        return LPF_FILTER_INIT_ERROR;

    SNDFILE* output_wav = sf_open(output_file_name, SFM_WRITE, &wav_info);

    if (output_wav == NULL)
    {
        eprintf("unable to open output file\n");
        return LPF_FILE_OPEN_ERROR;
    }

    while (samples_processed < samples_to_process)
    {
        sf_count_t samples_read =
            sf_read_float(input_wav, audio_buffer, lpf->buffer_size);

        filter_buffer(lpf, audio_buffer, samples_read);

        sf_count_t samples_written =
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

    return LPF_NO_ERROR;
}

enum lpf_error init_coeffs(low_pass_filter_t* lpf,
                           float sample_rate,
                           float cutoff,
                           enum window_t window_type)
{
    if (cutoff <= 0)
        return LPF_CUTOFF_ERROR;
    else if (sample_rate <= 0)
        return LPF_SAMPLE_RATE_ERROR;

    float transition_frequency = cutoff / sample_rate;

    for (int i = 0; i < lpf->order + 1; ++i)
    {
        if (i == lpf->order / 2.0f)
            lpf->filter_coeffs[i] = 2.0f * transition_frequency;
        else
        {
            float pi_x = (float)M_PI * (i - lpf->order / 2.0f);
            lpf->filter_coeffs[i] =
                sinf(2.0f * transition_frequency * pi_x) / pi_x;
        }
    }

    //bartlett_window(lpf->filter_coeffs, lpf->order + 1);

    // normalises coeffiecients to avoid clipping
    float sum = 0.0f;
    for (int i = 0; i < lpf->order + 1; ++i) sum += lpf->filter_coeffs[i];
    for (int i = 0; i < lpf->order + 1; ++i)
    {
        lpf->filter_coeffs[i] /= sum;
        printf("%f\n", lpf->filter_coeffs[i]);
    }
    return LPF_NO_ERROR;
}

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
            audio_buffer[i] += lpf->filter_coeffs[j] *
                               lpf->past_input_samples[get_read_point(lpf, j)];
        }

        increment_write_point(lpf);
    }
}

int get_read_point(low_pass_filter_t* lpf, int samples_delay)
{
    int read_point = lpf->write_point + lpf->order + 1 - samples_delay;
    if (read_point >= lpf->order + 1)
        read_point -= lpf->order + 1;

    return read_point;
}

void increment_write_point(low_pass_filter_t* lpf)
{
    ++lpf->write_point;
    if (lpf->write_point == lpf->order + 1)
        lpf->write_point = 0;
}

void lpf_destroy(low_pass_filter_t* lpf)
{
    if (lpf)
    {
        free(lpf->filter_coeffs);
        free(lpf->past_input_samples);
        free(lpf);
    }
}