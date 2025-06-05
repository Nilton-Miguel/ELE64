
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

        efeitoA.identificador = ECHO;
        efeitoA.parametro[0] = 1;
        efeitoA.parametro[1] = 0.69;
        efeitoA.parametro[2] = 0.6;
        efeitoA.parametro[3] = 0;

        efeitoB.identificador = ECHO;
        efeitoB.parametro[0] = 1;
        efeitoB.parametro[1] = 0.69;
        efeitoB.parametro[2] = 0.6;
        efeitoB.parametro[3] = 0;

        efeitoC.identificador = SOFT_AMP;
        efeitoC.parametro[0] = 1;
        efeitoC.parametro[1] = 0.4;
        efeitoC.parametro[2] = 0;
        efeitoC.parametro[3] = 0;

    FILE *file_preset;
    file_preset = fopen("preset_1.bin", "wb");
    escrever_preset(efeitoA, efeitoB, efeitoC, file_preset);
    fclose(file_preset);

    return 0;
}
