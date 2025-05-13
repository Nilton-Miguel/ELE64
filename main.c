
#include "geradores.h"

#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <fftw3.h>
#include <string.h>

#define SAMPLING_WINDOW_SIZE 256
int i;
char *preset_file_name = "presets/p001.txt";

int main(){

    /*
    FILE *preset_file;
    preset_file = fopen(preset_file_name, "r");
    char readBuffer[64];
    while(fgets(readBuffer, 64, preset_file)){

        char identifier[4];
        strncpy(identifier, readBuffer, 4);
        if(strcmp(identifier, "e001")) printf("efeito 1\n");
    } 
    fclose(preset_file);
    */

    fftw_complex * resposta_efeito_1;
    resposta_efeito_1 = (fftw_complex*) fftw_malloc(SAMPLING_WINDOW_SIZE * sizeof(fftw_complex));
    resposta_efeito_1 = resposta_h_media_movel(SAMPLING_WINDOW_SIZE, 100);

    printf("\n");
    for(i=0; i<SAMPLING_WINDOW_SIZE; i++){

        printf("%d: (%f)+j(%f)\n", i, creal(resposta_efeito_1[i]), cimag(resposta_efeito_1[i]));
    }

    fftw_free(resposta_efeito_1);
    return 0;
}
