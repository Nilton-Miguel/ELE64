
#ifndef GERADORES_H
#define GERADORES_H

#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <fftw3.h>

void resposta_h_buffer(fftw_complex *vetor, int SAMPLING_WINDOW_SIZE);

void resposta_h_media_movel(fftw_complex *vetor, int SAMPLING_WINDOW_SIZE, int ORDER);

#endif
