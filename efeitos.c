
#include "efeitos.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void buffer(float *sinal, float *output, int LENGTH)
{
    // memoria residual
    // x : 0
    // y : 0

    long int j;
    for(j=0; j<LENGTH; j++) 
    
        output[j] = sinal[j];
}
void amplificador_soft(float *sinal, float *output, int LENGTH, float GANHO)
{
    // memoria residual
    // x : 0
    // y : 0

    long int j;
    for(j=0; j<LENGTH; j++)

        output[j] = tanh(GANHO*sinal[j]);
}
void amplificador_hard(float *sinal, float *output, int LENGTH, float GANHO)
{
    // memoria residual
    // x : 0
    // y : 0

    long int j;
    for(j=0; j<LENGTH; j++)
    {
        output[j] = sinal[j] * GANHO;
        if (output[j] >= 1)        output[j] = 1;
        else if (output[j] <= -1)  output[j] = -1;
    }
}
void saturador_soft(float *sinal, float *output, int LENGTH, float JANELA, float GANHO)
{
    // memoria residual
    // x : 0
    // y : 0

    float ganho_argumento = GANHO / JANELA ;

    long int j;
    for(j=0; j<LENGTH; j++)

        output[j] = JANELA * tanh(ganho_argumento*sinal[j]);
}
void saturador_hard(float *sinal, float *output, int LENGTH, float JANELA, float GANHO)
{
    // memoria residual
    // x : 0
    // y : 0

    long int j;
    for(j=0; j<LENGTH; j++)
    {
        output[j] = sinal[j] * GANHO;
        if (output[j] >= JANELA)        output[j] = JANELA;
        else if (output[j] <= -JANELA)  output[j] = -JANELA;
    }
}
void echo(float *sinal, float *output, float *y_residual, long int * indexador_externo, int LENGTH, float DURATION, long int sampling_rate, float DECAY, float WET)
{
    // memoria residual
    // x : 0
    // y : DURATION

    // converte DURATION de segundos para amostras
    long int SAMPLE_DURATION = (long int)(DURATION * sampling_rate);
    //int mod_j = 0;
    float DRY = 1 - WET;

    long int j;
    for(j=0; j<LENGTH; j++)
    {
        //mod_j = (SAMPLE_DURATION + j) % SAMPLE_DURATION;
        //printf("J: %ld EX: %ld\n", j, *(indexador_externo));

        output[j] = WET*(sinal[j] + DECAY * y_residual[*(indexador_externo)]) + DRY*(sinal[j]);
        y_residual[*(indexador_externo)] = output[j];

        *(indexador_externo) += 1;
        if (*(indexador_externo) >= SAMPLE_DURATION) *(indexador_externo) -= SAMPLE_DURATION;
    }
}
void lowpass(float *sinal, float *output, float *y_residual, long int sampling_rate, int LENGTH, float ANALOG_FREQUENCY, float WET)
{
    // memoria residual
    // x : 0
    // y : 1

    float DRY = 1 - WET;

    float ANALOG_ANGULAR_FREQUENCY = 2 * M_PI * ANALOG_FREQUENCY;
    float ANTI_INERCIA = ANALOG_ANGULAR_FREQUENCY / (sampling_rate + ANALOG_ANGULAR_FREQUENCY);
    float INERCIA = 1 - ANTI_INERCIA;

    long int j;
    for(j=0; j<LENGTH; j++)
    {
        output[j] = WET*(ANTI_INERCIA * sinal[j] + INERCIA * y_residual[0]) + DRY*(sinal[j]);
        y_residual[0] = output[j];
    }
}
void highpass(float *sinal, float *output, float *x_residual, float *y_residual, long int sampling_rate, int LENGTH, float ANALOG_FREQUENCY, float WET)
{
    // memoria residual
    // x : 1
    // y : 1

    float DRY = 1 - WET;

    float FACTOR = sampling_rate / (sampling_rate + 2 * M_PI * ANALOG_FREQUENCY);

    long int j;
    for(j=0; j<LENGTH; j++)
    {
        output[j] = WET*(FACTOR * ( sinal[j] - x_residual[0] + y_residual[0])) + DRY*(sinal[j]);
        x_residual[0] = sinal[j];
        y_residual[0] = output[j];
    }
}
void notch(float *sinal, float *output, float *x_residual, float *y_residual, long int *indexador_externo, long int sampling_rate, int LENGTH, float ANALOG_FREQUENCY, float WET)
{
    // memoria residual
    // x : 2
    // y : 2

    float DRY = 1 - WET;

    float RADIUS = 0.95;

    float A = 2 * cos(2*M_PI*(ANALOG_FREQUENCY / sampling_rate));
    float B = (1 + pow(RADIUS, 2)) * cos(2*M_PI*(ANALOG_FREQUENCY / sampling_rate));
    float C = pow(RADIUS, 2);

    float NORMA = (1 - B + C) / (2 - A);

    int passado;
    int antepassado;

    long int j;
    for(j=0; j<LENGTH; j++)
    {
        // indexador externo é usado para calcular os indices modulares
        passado     = (2 + *(indexador_externo)) % 2;
        antepassado = (3 + *(indexador_externo)) % 2;

        //printf("%ld : passado: %d antepassado: %d\n", *(indexador_externo), passado, antepassado);

        output[j] = WET*(NORMA*(sinal[j] - A*x_residual[passado] + x_residual[antepassado]) +B*y_residual[passado] - C*y_residual[antepassado]) + DRY*(sinal[j]);

        x_residual[antepassado] = sinal[j];
        y_residual[antepassado] = output[j];

        // j incrementa indexador_externo, que vai de 0 a 999 (essa abordagem funcionou melhor para multiplos ponteiros relativos)
        *(indexador_externo) += 1;
        if (*(indexador_externo) >= 1000) *(indexador_externo) -= 1000;
    }
}
