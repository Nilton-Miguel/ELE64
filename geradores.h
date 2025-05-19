
#ifndef GERADORES_H
#define GERADORES_H

#include <complex.h>
#include <fftw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct wav_header{

        char    riff[4];
        int32_t flength;
        char    wave[4];
        char    fmt[4];
        int32_t chunk_size;
        int16_t format_tag;
        int16_t num_chans;
        int32_t sample_rate;
        int32_t bytes_per_second;
        int16_t bytes_per_sample;
        int16_t bits_per_sample;
        char    data[4];
        int32_t dlength;
    }wav_header;

void resposta_h_amplificador(fftw_complex *vetor, int SAMPLING_WINDOW_SIZE, float ganho);

void resposta_h_media_movel(fftw_complex *vetor, int SAMPLING_WINDOW_SIZE, int ORDER);

void frequencia_pura(fftw_complex *vector, int LENGTH, int SAMPLING_RATE, float ANALOG_FREQUENCY, float AMPLITUDE, float PHASE);

void rebaixar_16bits(fftw_complex *vetor__double_double, short int *vetor_short_int, int LENGTH);

void adicionar_ruido(fftw_complex *vetor, int LENGTH, float AMPLITUDE);

#endif
