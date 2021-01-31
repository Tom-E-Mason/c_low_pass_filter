
#include "low_pass_filter.h"

typedef struct low_pass_filter
{
    int order;
    float* filter_coeffs;
    float* past_input_samples;
    int write_point;
    enum window_t window_type;
} low_pass_filter_t;

low_pass_filter_t* lpf_create(float sample_rate, float cutoff, enum window_t window_type)
{
    low_pass_filter_t* lpf = (low_pass_filter_t*)malloc(sizeof(low_pass_filter_t));
    if (lpf)
    {
        lpf->order = 126;
        lpf->filter_coeffs = (float*)calloc(((size_t)lpf->order + 1), sizeof(float));
        lpf->past_input_samples = (float*)calloc(((size_t)lpf->order + 1), sizeof(float));
        lpf->write_point = 0;
        lpf->window_type = window_type;
    }

    return lpf;
}

int lpf_filter_file(low_pass_filter_t * lpf,
                    const char * input_file,
                    const char * output_file,
                    float cutoff,
                    enum window_t window_type,
                    int* samples_filtered)
{


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