
#ifndef GERADORES_H
#define GERADORES_H

#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <fftw3.h>

void resposta_h_amplificador(fftw_complex *vetor, int SAMPLING_WINDOW_SIZE, float ganho);

void resposta_h_media_movel(fftw_complex *vetor, int SAMPLING_WINDOW_SIZE, int ORDER);

#endif
