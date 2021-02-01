
#pragma once

void bartlett_window(float* coeffs, int num_coeffs)
{
    float order = (float)num_coeffs - 1.0f;
    for (int i = 0; i < num_coeffs; ++i)
    {
        coeffs[i] *= 1 - 2.0f * (fabsf((float)i - order / 2.0f) / order);
        printf("%f\n", coeffs[i]);
    }
}