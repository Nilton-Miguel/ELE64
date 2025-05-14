
#include "geradores.h"

#include <complex.h>
#include <fftw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define SAMPLING_WINDOW_SIZE 1024
int i;

int main(){

    // alocar memoria para sinal
    fftw_complex * sinal;
    sinal = (fftw_complex*) fftw_malloc(SAMPLING_WINDOW_SIZE * sizeof(fftw_complex));

    // alocar memoria para saida
    fftw_complex * saida;
    saida = (fftw_complex*) fftw_malloc(SAMPLING_WINDOW_SIZE * sizeof(fftw_complex));

    // alocar memoria para os efeitos
    fftw_complex * h;
    h = (fftw_complex*) fftw_malloc(SAMPLING_WINDOW_SIZE * sizeof(fftw_complex));

    // calcular as DFT dos efeitos
    fftw_plan plano_dft_h;
    plano_dft_h = fftw_plan_dft_1d(SAMPLING_WINDOW_SIZE, h, h, FFTW_FORWARD, FFTW_MEASURE);
    resposta_h_buffer(h, SAMPLING_WINDOW_SIZE);
    fftw_execute(plano_dft_h);

    // preparar os planos para o sinal
    fftw_plan plano_dft_sinal;
    fftw_plan plano_idft_sinal;

    plano_dft_sinal =  fftw_plan_dft_1d(SAMPLING_WINDOW_SIZE, sinal, saida, FFTW_FORWARD,  FFTW_MEASURE);
    plano_idft_sinal = fftw_plan_dft_1d(SAMPLING_WINDOW_SIZE, saida, saida, FFTW_BACKWARD, FFTW_MEASURE);

    // futuro loop logico da aplicacao -----------------------------------------------------------------------------------

    // amostragem do sinal
    for(i=0;i<SAMPLING_WINDOW_SIZE;i++){

        sinal[i] = sin(2*M_PI*i/(SAMPLING_WINDOW_SIZE/2)) + 0.05*(float)rand()/(float)(RAND_MAX);
    }

    // executar a DFT do sinal e produto com os efeitos
    fftw_execute(plano_dft_sinal);
    for(i=0;i<SAMPLING_WINDOW_SIZE;i++) saida[i] *= h[i];

    // executar a IDFT do sinal e normalizar o vetor
    fftw_execute(plano_idft_sinal);
    for(i=0;i<SAMPLING_WINDOW_SIZE;i++) saida[i] /= SAMPLING_WINDOW_SIZE;
    // -------------------------------------------------------------------------------------------------------------------

    // plotagem do sinal pre e pos filtro
    //------------------------------------------------------------------------------------------------------------------
    FILE *fp = NULL;
    FILE *gnupipe = NULL;
    char *GnuCommands [] = {"plot 'data.tmp' using 1:2 with points pt 7 ps 0.3, '' using 1:3 with points pt 7 ps 0.3"};

    fp = fopen("data.tmp", "w");
    gnupipe = popen("gnuplot -persistent", "w");
    //------------------------------------------------------------------------------------------------------------------
    printf("\n");
    for(i=0; i<SAMPLING_WINDOW_SIZE; i++){

        fprintf(fp, "%d %f %f\n", i, creal(sinal[i]), creal(saida[i]));
    }
    // invocar o gnuplot
    fprintf(gnupipe, "%s\n", GnuCommands[0]);
    //------------------------------------------------------------------------------------------------------------------


    // desalocar tudo
    fftw_free(h);
    fftw_free(sinal);
    fftw_free(saida);

    fftw_destroy_plan(plano_dft_sinal);
    fftw_destroy_plan(plano_idft_sinal);
    fftw_destroy_plan(plano_dft_h);

    return 0;
}
