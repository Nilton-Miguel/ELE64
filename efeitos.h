
#ifndef EFEITOS_H
#define EFEITOS_H

#include <complex.h>
//#include <fftw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void buffer(float *sinal, float *output, int LENGTH);

void amplificador_soft(float *sinal, float *output, int LENGTH, float GANHO);

void amplificador_hard(float *sinal, float *output, int LENGTH, float GANHO);

void saturador_soft(float *sinal, float *output, int LENGTH, float JANELA, float GANHO);

void saturador_hard(float *sinal, float *output, int LENGTH, float JANELA, float GANHO);

void echo(float *sinal, float *output, float *y_residual, long int *indexador_externo, int LENGTH, float DURATION, long int sampling_rate, float DECAY, float WET);

void lowpass(float *sinal, float *output, float *y_residual, long int sampling_rate, int LENGTH, float ANALOG_FREQUENCY, float WET);

void highpass(float *sinal, float *output, float *x_residual, float *y_residual, long int sampling_rate, int LENGTH, float ANALOG_FREQUENCY, float WET);

void notch(float *sinal, float *output, float *x_residual, float *y_residual, long int *indexador_externo, long int sampling_rate, int LENGTH, float ANALOG_FREQUENCY, float WET);

#endif
