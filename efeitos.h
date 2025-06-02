
#ifndef EFEITOS_H
#define EFEITOS_H

#include <complex.h>
#include <fftw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void saturador_soft(float *sinal, float *output, int LENGTH, double JANELA, float GANHO);

void saturador_hard(float *sinal, float *output, int LENGTH, double JANELA, float GANHO);

void echo(float *sinal, float *output, float *residual, int LENGTH, long int DURATION, float DECAY);

void lowpass(float *sinal, float *output, long int sampling_rate, int LENGTH, float ANALOG_FREQUENCY);

void highpass(float *sinal, float *output, long int sampling_rate, int LENGTH, float ANALOG_FREQUENCY);

void notch(float *sinal, float *output, float *x_residual, float *y_residual, long int sampling_rate, int LENGTH, float ANALOG_FREQUENCY, float RADIUS);

#endif
