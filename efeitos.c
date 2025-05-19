#include "efeitos.h"

#include <complex.h>
#include <fftw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void saturador_soft(fftw_complex *sinal, fftw_complex *output, int LENGTH, double JANELA, float GANHO){

    float ganho_argumento = GANHO / JANELA ;

    int j;
    for(j=0; j<LENGTH; j++)

        output[j] = JANELA * tanh(ganho_argumento*sinal[j]);
}
void saturador_hard(fftw_complex *sinal, fftw_complex *output, int LENGTH, double JANELA, float GANHO){

    float offset_argumento  = JANELA / GANHO ;
    float ganho_saida       = GANHO / 2 ;

    int j;
    for(j=0; j<LENGTH; j++)

        output[j] = ganho_saida * (abs( sinal[j] + offset_argumento ) - abs( sinal[j] - offset_argumento ));
}
