
#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdlib.h>
#include <stdio.h>

// ---- MNEUMONICO -------------- PARAMETROS CONFIGURAVEIS -----------------
#define BUFFER      0x0 
#define SOFT_AMP    0x1 // [0] GANHO
#define HARD_AMP    0x2 // [0] GANHO
#define SOFT_SAT    0x3 // [0] GANHO    [1] JANELA
#define HARD_SAT    0x4 // [0] GANHO    [1] JANELA
#define ECHO        0x5 // [0] WET      [1] DURATION    [2] DECAY
#define LOWPASS     0x6 // [0] WET      [1] CUTOFF
#define HIGHPASS    0x7 // [0] WET      [1] CUTOFF
#define NOTCH       0x8 // [0] WET      [1] CUTOUT
// -------------------------------------------------------------------------

#define NUMBER_OF_PARAMETERS 4

    typedef struct{

        char identificador;
        float parametro[NUMBER_OF_PARAMETERS];

    }type_effect;

void inicializar_efeito(type_effect *efeito);

void imprime_efeito(type_effect *efeito);

#endif
