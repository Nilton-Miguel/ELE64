
#include "interface.h"

#include <stdlib.h>
#include <stdio.h>

void inicializar_efeito(type_effect *efeito)
{
    efeito -> identificador = 0x0;
    int j;
    for(j=0; j<NUMBER_OF_PARAMETERS; j++) efeito -> parametro[j] = 0.0;
}
void imprime_efeito(type_effect *efeito)
{
    printf("identificador: 0x%d ", efeito -> identificador);
    printf("parametros: ");
    int j;
    for(j=0; j<NUMBER_OF_PARAMETERS; j++) printf("%f ", efeito -> parametro[j]);
    
    printf("\n");
}
