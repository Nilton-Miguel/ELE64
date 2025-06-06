
#include "interface.h"
#include "efeitos.h"

#include <sndfile.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main(){

    type_effect efeitoA, efeitoB, efeitoC;

    inicializar_efeito(&efeitoA);
    inicializar_efeito(&efeitoB);
    inicializar_efeito(&efeitoC);

        efeitoA.identificador = LOWPASS;
        efeitoA.parametro[0] = 1;
        efeitoA.parametro[1] = 5;
        efeitoA.parametro[2] = 0;
        efeitoA.parametro[3] = 0;

        efeitoB.identificador = HARD_AMP;
        efeitoB.parametro[0] = 500;
        efeitoB.parametro[1] = 0;
        efeitoB.parametro[2] = 0;
        efeitoB.parametro[3] = 0;

        efeitoC.identificador = LOWPASS;
        efeitoC.parametro[0] = 1;
        efeitoC.parametro[1] = 5;
        efeitoC.parametro[2] = 0;
        efeitoC.parametro[3] = 0;

    FILE *file_preset;
    file_preset = fopen("preset_2.bin", "wb");
    escrever_preset(efeitoA, efeitoB, efeitoC, file_preset);
    fclose(file_preset);

    return 0;
}
