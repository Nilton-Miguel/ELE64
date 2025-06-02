
#ifndef GERADORES_H
#define GERADORES_H

#include <complex.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void resposta_h_amplificador(float *vetor, int SAMPLING_WINDOW_SIZE, float ganho);

void resposta_h_media_movel(float *vetor, int SAMPLING_WINDOW_SIZE, int ORDER);

void frequencia_pura(float *vector, int LENGTH, int SAMPLING_RATE, float ANALOG_FREQUENCY, float AMPLITUDE);

void adicionar_frequencia_pura(float *vetor, int LENGTH, int SAMPLING_RATE, float ANALOG_FREQUENCY, float AMPLITUDE);

void adicionar_ruido(float *vetor, int LENGTH, float AMPLITUDE);

#endif
