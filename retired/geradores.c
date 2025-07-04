
#include "geradores.h"

#include <complex.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <limits.h>

/*  --------------------------------------------------------------------------------------------------------------

    autor: Nilton Miguel Guimaraes de Souza
    info:  Brasil Parana Curitiba 23/05/2025

    Esta biblioteca foi desenvolvida para fins academicos na disciplina "ELE64 - Oficina de Integracao UTFPR CT"

    Nesta biblioteca consta um conjunto de metodos para auto geracao de resposta a impulso de alguns filtros e 
    efeitos, para que possamos obter esses vetores sem a necessidade de escrever manualmente, o que acarretaria
    a perda de escalabilidade caso o tamanho da janela de processamento do sinal fosse alterada.
    --------------------------------------------------------------------------------------------------------------

    author: Nilton Miguel Guimaraes de Souza
    info:   Brasil Parana Curitiba 23/05/2025

    This library was developed with academic objectives for the course "ELE64 - Oficina de Integracao UTFPR CT"

    In it is contained a set of methods for auto generation of impulse response of some filters and audio effects,
    so we could obtain these vectors of values with no need to write them manually, which would cause loss of
    scalabily if the size of the processing window were altered. 
    -------------------------------------------------------------------------------------------------------------- */

void resposta_h_amplificador(float *vetor, int SAMPLING_WINDOW_SIZE, float ganho)
{

    // limpar a regiao de memoria
    int j;
    for(j=0; j<SAMPLING_WINDOW_SIZE; j++) 
    
        vetor[j] = 0;

    // atualizar todos os coeficientes nao nulos
    vetor[0] = ganho;
}
void resposta_h_media_movel(float *vetor, int SAMPLING_WINDOW_SIZE, int ORDER)
{
    // limpar a regiao de memoria
    int j;
    for(j=0; j<SAMPLING_WINDOW_SIZE; j++) 
    
        vetor[j] = 0;

    // atualizar todos os coeficientes nao nulos
    double coeficiente = 1.0f / ORDER;
    vetor[0] = coeficiente;
    for(j=0; j<ORDER; j++) 

        vetor[SAMPLING_WINDOW_SIZE -1 -j] = coeficiente;
}
void frequencia_pura(float *vetor, int LENGTH, int SAMPLING_RATE, float ANALOG_FREQUENCY, float AMPLITUDE)
{
    double digital_frequency = (ANALOG_FREQUENCY / SAMPLING_RATE) * 2 * M_PI;

    srand(time(0));
    const float PHASE = 60*(float)rand()/(float)(RAND_MAX);

    int j;
    for(j=0; j<LENGTH; j++)

        vetor[j] = AMPLITUDE * cos(digital_frequency*j + PHASE);

}
void adicionar_frequencia_pura(float *vetor, int LENGTH, int SAMPLING_RATE, float ANALOG_FREQUENCY, float AMPLITUDE)
{
    double digital_frequency = (ANALOG_FREQUENCY / SAMPLING_RATE) * 2 * M_PI;

    srand(time(0));
    const float PHASE = 60*(float)rand()/(float)(RAND_MAX);

    int j;
    for(j=0; j<LENGTH; j++)

        vetor[j] += AMPLITUDE * cos(digital_frequency*j + PHASE);

}
void adicionar_ruido(float *vetor, int LENGTH, float AMPLITUDE)
{
    int j;
    for(j=0; j<LENGTH; j++)

        vetor[j] += AMPLITUDE * (float)rand()/(float)(RAND_MAX);
}
