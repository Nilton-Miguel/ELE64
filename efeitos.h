
#ifndef EFEITOS_H
#define EFEITOS_H

#include <complex.h>
#include <fftw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void saturador_soft(fftw_complex *sinal, fftw_complex *output, int LENGTH, double JANELA, float GANHO);

void saturador_hard(fftw_complex *sinal, fftw_complex *output, int LENGTH, double JANELA, float GANHO);

void delay(fftw_complex *sinal, fftw_complex *output, fftw_complex *residual, int LENGTH, long int DURATION, float DECAY);

void passive_lowpass(fftw_complex *sinal, fftw_complex *output, long int sampling_rate, int LENGTH, float ANALOG_FREQUENCY);

#endif
