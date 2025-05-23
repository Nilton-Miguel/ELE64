#include "efeitos.h"

#include <complex.h>
#include <fftw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void saturador_soft(fftw_complex *sinal, fftw_complex *output, int LENGTH, double JANELA, float GANHO)
{
    float ganho_argumento = GANHO / JANELA ;

    int j;
    for(j=0; j<LENGTH; j++)

        output[j] = JANELA * tanh(ganho_argumento*sinal[j]);
}
void saturador_hard(fftw_complex *sinal, fftw_complex *output, int LENGTH, double JANELA, float GANHO)
{
    float offset_argumento  = JANELA / GANHO ;
    float ganho_saida       = GANHO / 2 ;

    int j;
    for(j=0; j<LENGTH; j++)

        output[j] = ganho_saida * (abs( sinal[j] + offset_argumento ) - abs( sinal[j] - offset_argumento ));
}
void delay(fftw_complex *sinal, fftw_complex *output, fftw_complex *residual, int LENGTH, long int DURATION, float DECAY)
{
    int j;
    for(j=0; j<LENGTH; j++)
    {
        int mod_j = (DURATION + j) % DURATION;

        output[j] = sinal[j] + DECAY * residual[mod_j];
        residual[mod_j] = output[j];
    }
}
void passive_lowpass(fftw_complex *sinal, fftw_complex *output, long int sampling_rate, int LENGTH, float ANALOG_FREQUENCY)
{
    fftw_complex residual = 0;

    float ANALOG_ANGULAR_FREQUENCY = 2 * M_PI * ANALOG_FREQUENCY;
    float ANTI_INERCIA = ANALOG_ANGULAR_FREQUENCY / (sampling_rate + ANALOG_ANGULAR_FREQUENCY);
    float INERCIA = 1 - ANTI_INERCIA;

    int j;
    for(j=0; j<LENGTH; j++)
    {
        output[j] = ANTI_INERCIA * sinal[j] + INERCIA * residual;
        residual = output[j];
    }
}
void notch()
{

}
void chorus()
{

}
void highpass()
{

}
void bandpass()
{

}
void bandreject()
{

}
void reverb()
{
    
}
