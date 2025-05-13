
#include "geradores.h"

#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <fftw3.h>

#define SAMPLING_WINDOW_SIZE 256
int i;

int main(){

    fftw_complex * resposta_media_movel;
    resposta_media_movel = (fftw_complex*) fftw_malloc(SAMPLING_WINDOW_SIZE * sizeof(fftw_complex));

    resposta_media_movel = media_movel(SAMPLING_WINDOW_SIZE, 50);

    printf("\n");
    for(i=0; i<SAMPLING_WINDOW_SIZE; i++){

        printf("%d: (%f)+j(%f)\n", i, creal(resposta_media_movel[i]), cimag(resposta_media_movel[i]));
    }

    fftw_free(resposta_media_movel);
    return 0;
}
