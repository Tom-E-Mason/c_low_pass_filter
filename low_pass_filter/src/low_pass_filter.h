
#pragma once

#include <math.h>
#include <sndfile.h>
#include <stdlib.h>

#define eprintf(...) fprintf(stderr, __VA_ARGS__)

typedef struct low_pass_filter low_pass_filter_t;

enum window_t
{
    NULL_WINDOW,
    KAISER,
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
    LPF_FILTER_INIT_ERROR,
    LPF_FILTER_FILE_ERROR,
    LPF_FILE_WRITE_ERROR,
};

low_pass_filter_t* lpf_create(float cutoff, enum window_t window_type, size_t buffer_size);

enum lpf_error lpf_filter_file(low_pass_filter_t* lpf,
                               const char* input_file,
                               const char* output_file,
                               enum window_t window_type,
                               sf_count_t* frames_filtered);

void lpf_destroy(low_pass_filter_t* lpf);
