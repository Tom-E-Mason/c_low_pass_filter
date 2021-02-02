
#pragma once

#include <math.h>

void bartlett_window(float* coeffs, int num_coeffs);
void blackman_window(float* coeffs, int num_coeffs);
void hamming_window(float* coeffs, int num_coeffs);
void hanning_window(float* coeffs, int num_coeffs);
void kaiser_window(float* coeffs, int num_coeffs);