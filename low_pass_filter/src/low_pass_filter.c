
#include "low_pass_filter.h"

typedef struct low_pass_filter
{
    int order;
    float* filter_coeffs;
    float* past_input_samples;
    int write_point;
    enum window_t window_type;
    size_t buffer_size;
} low_pass_filter_t;

enum window_t lpf_init_coeffs(low_pass_filter_t* lpf,
                              float sample_rate,
                              float cutoff,
                              enum window_t window_type);

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
                               const char* input_file,
                               const char* output_file,
                               float cutoff,
                               enum window_t window_type,
                               int* samples_filtered)
{
    SF_INFO wav_info;
    SNDFILE* input_wav = sf_open(input_file, SFM_READ, &wav_info);

    if (input_wav == NULL)
    {
        eprintf("unable to open input file\n");
        return LPF_FILE_OPEN_ERROR;
    }

    sf_count_t samples_to_process = wav_info.frames;
    sf_count_t samples_processed = 0;

    const int buffer_size = 512;
    float* audio_buffer = (float*)calloc(lpf->buffer_size, sizeof(float));

    lpf_init_coeffs(lpf, wav_info.samplerate, cutoff, window_type);

    return LPF_NO_ERROR;
}

enum lpf_error lpf_init_coeffs(low_pass_filter_t* lpf,
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
            lpf->filter_coeffs[i] = 2 * transition_frequency;
        else
        {
            float x = i - lpf->order / 2.0;
            lpf->filter_coeffs[i] =
                sin(2 * M_PI * transition_frequency * x) / (M_PI * x);
        }
    }

    // normalises coeffiecients to avoid clipping
    float sum = 0.0f;
    for (int i = 0; i < lpf->order + 1; ++i) 
        sum += lpf->filter_coeffs[i];
    for (int i = 0; i < lpf->order + 1; ++i) 
        lpf->filter_coeffs[i] /= sum;

    return LPF_NO_ERROR;
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