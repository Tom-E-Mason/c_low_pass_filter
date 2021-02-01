
#pragma once

#include <sndfile.h>
#include <stdlib.h>
#include <math.h>

#define eprintf(...) fprintf(stderr, __VA_ARGS__)

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
    LPF_FILE_OPEN_ERROR,
    LPF_CUTOFF_ERROR,
    LPF_SAMPLE_RATE_ERROR,
};

low_pass_filter_t* lpf_create(float cutoff,
                              enum window_t window_type,
                              size_t buffer_size);

enum lpf_error lpf_filter_file(low_pass_filter_t* lpf,
                               const char* input_file,
                               const char* output_file,
                               float cutoff,
                               enum window_t window_type,
                               int* samples_filtered);

void lpf_destroy(low_pass_filter_t* lpf);
