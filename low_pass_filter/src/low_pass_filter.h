
#pragma once

#include <stdlib.h>

typedef struct low_pass_filter low_pass_filter_t;

enum window_t
{
    NULL_WINDOW,
    KAISSER,
    HAMMING,
    HANNING,
    BLACKMAN,
    BARTLETT,
    RECTANGULAR,
};

enum lpf_error
{
    LPF_NO_ERROR,
};

low_pass_filter_t* lpf_create(float sample_rate,
                              float cutoff,
                              enum window_t window_type);

enum lpf_error lpf_filter_file(low_pass_filter_t* lpf,
                               const char* input_file,
                               const char* output_file,
                               float cutoff,
                               enum window_t window_type,
                               int* samples_filtered);

void lpf_destroy(low_pass_filter_t* lpf);
