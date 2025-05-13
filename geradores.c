
#include "geradores.h"

#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <fftw3.h>

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

fftw_complex *media_movel(int SAMPLING_WINDOW_SIZE, int ORDER){

    fftw_complex *devolutiva;
    devolutiva = (fftw_complex*) fftw_malloc(SAMPLING_WINDOW_SIZE * sizeof(fftw_complex));

    // limpar a regiao de memoria
    int j;
    for(j=0; j<SAMPLING_WINDOW_SIZE; j++) devolutiva[j] = 0;

    // atualizar todos os coeficientes nao nulos
    double coeficiente = 1.0f / ORDER;
    devolutiva[0] = coeficiente;
    for(j=0; j<ORDER; j++) devolutiva[SAMPLING_WINDOW_SIZE -1 -j] = coeficiente;

    return devolutiva;
}
