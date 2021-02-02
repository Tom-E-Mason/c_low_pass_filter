
#include "window_functions.h"

void bartlett_window(float* coeffs, int num_coeffs)
{
    const float order = (float)num_coeffs - 1.0f;
    for (int i = 0; i < num_coeffs; ++i)
        coeffs[i] *= 1.0f - 2.0f * (fabsf((float)i - order / 2.0f) / order);
}

void blackman_window(float* coeffs, int num_coeffs)
{
    const float order = (float)num_coeffs - 1.0f;
    for (int i = 0; i < num_coeffs; ++i)
        coeffs[i] *= 0.42f - 0.5f * cosf(2 * M_PI * i / order) +
                     0.08f * cosf(4 * M_PI * i / order);
}

void hamming_window(float* coeffs, int num_coeffs)
{
    const float order = (float)num_coeffs - 1.0f;
    for (int i = 0; i < num_coeffs; ++i)
        coeffs[i] *= 0.54f - 0.46f * cosf(2 * M_PI * i / order);
}

void hanning_window(float* coeffs, int num_coeffs)
{
    const float order = (float)num_coeffs - 1.0f;
    for (int i = 0; i < num_coeffs; ++i)
        coeffs[i] *= 0.5f - 0.5f * cosf(2 * M_PI * i / order);
}

float bessel_zero(float x)
{
    float bessel_z = 1;
    int factorial = 1;

    for (int i = 1; i <= 20; ++i)
    {
        factorial *= i;
        bessel_z += powf(x / 2.0f, 2.0f * (float)i) / powf(factorial, 2.0f);
    }

    return bessel_z;
}

void kaiser_window(float* coeffs, int num_coeffs)
{
    const float beta = 2.0f * sqrtf(2.0f * M_PI);
    const float order = (float)num_coeffs - 1.0f;
    const float bessel_z_beta = bessel_zero(beta);

    for (int i = 0; i < num_coeffs; ++i)
    {
        coeffs[i] *=
            bessel_zero(
                beta *
                sqrtf(1.0f - powf((2.0f * (float)i / order) - 1.0f, 2.0f))) /
            bessel_z_beta;
    }
}