
#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdlib.h>
#include <stdio.h>

#define NUMBER_OF_PARAMETERS 4

    typedef struct{

        char identificador;
        float parametro[NUMBER_OF_PARAMETERS];

    }type_effect;

void inicializar_efeito(type_effect *efeito);

void imprime_efeito(type_effect *efeito);

#endif
