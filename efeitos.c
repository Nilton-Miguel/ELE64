#include "efeitos.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void saturador_soft(float *sinal, float *output, int LENGTH, double JANELA, float GANHO)
{
    float ganho_argumento = GANHO / JANELA ;

    long int j;
    for(j=0; j<LENGTH; j++)

        output[j] = JANELA * tanh(ganho_argumento*sinal[j]);
}
void saturador_hard(float *sinal, float *output, int LENGTH, double JANELA, float GANHO)
{
    float offset_argumento  = JANELA / GANHO ;
    float ganho_saida       = GANHO / 2 ;

    long int j;
    for(j=0; j<LENGTH; j++)

        output[j] = ganho_saida * (abs( sinal[j] + offset_argumento ) - abs( sinal[j] - offset_argumento ));
}
void echo(float *sinal, float *output, float *residual, int LENGTH, long int DURATION, float DECAY)
{
    long int j;
    for(j=0; j<LENGTH; j++)
    {
        int mod_j = (DURATION + j) % DURATION;

        output[j] = sinal[j] + DECAY * residual[mod_j];
        residual[mod_j] = output[j];
    }
}
void lowpass(float *sinal, float *output, long int sampling_rate, int LENGTH, float ANALOG_FREQUENCY)
{
    float residual = 0;

    float ANALOG_ANGULAR_FREQUENCY = 2 * M_PI * ANALOG_FREQUENCY;
    float ANTI_INERCIA = ANALOG_ANGULAR_FREQUENCY / (sampling_rate + ANALOG_ANGULAR_FREQUENCY);
    float INERCIA = 1 - ANTI_INERCIA;

    long int j;
    for(j=0; j<LENGTH; j++)
    {
        output[j] = ANTI_INERCIA * sinal[j] + INERCIA * residual;
        residual = output[j];
    }
}
void highpass(float *sinal, float *output, long int sampling_rate, int LENGTH, float ANALOG_FREQUENCY)
{
    float x_residual = 0;
    float y_residual = 0;

    float FACTOR = sampling_rate / (sampling_rate + 2 * M_PI * ANALOG_FREQUENCY);

    long int j;
    for(j=0; j<LENGTH; j++)
    {
        output[j] = FACTOR * ( sinal[j] - x_residual + y_residual );
        x_residual = sinal[j];
        y_residual = output[j];
    }
}
void notch(float *sinal, float *output, float *x_residual, float *y_residual, long int sampling_rate, int LENGTH, float ANALOG_FREQUENCY, float RADIUS)
{
    float A = 2 * cos(2*M_PI*(ANALOG_FREQUENCY / sampling_rate));
    float B = (1 + pow(RADIUS, 2)) * cos(2*M_PI*(ANALOG_FREQUENCY / sampling_rate));
    float C = pow(RADIUS, 2);

    float NORMA = (1 - B + C) / (2 - A);

    //printf("%f %f %f %f\n", A, B, C, NORMA);

    int passado;
    int antepassado;

    long int j;
    for(j=0; j<LENGTH; j++)
    {
        passado     = (2 + j) % 2;
        antepassado = (3 + j) % 2;

        //printf("%f %f %f %f\n", creal(x_residual[passado]), creal(x_residual[antepassado]), creal(y_residual[passado]), creal(y_residual[antepassado]));

        output[j] = NORMA*(sinal[j] - A*x_residual[passado] + x_residual[antepassado]) +B*y_residual[passado] - C*y_residual[antepassado];

        x_residual[antepassado] = sinal[j];
        y_residual[antepassado] = output[j];
        //printf("%f %f\n", creal(sinal[j]), creal(output[j]));
    }
}
