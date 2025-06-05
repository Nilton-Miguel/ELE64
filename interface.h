
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

        float *x_residual_esquerdo;
        float *y_residual_esquerdo;

        float *x_residual_direito;
        float *y_residual_direito;

    }type_effect;

void inicializar_efeito(type_effect *efeito);

void imprime_efeito(type_effect *efeito);

void switch_process(type_effect *efeito, float *entrada_esquerdo, float *entrada_direito, float *saida_esquerdo, float *saida_direito, long int signal_length, long int sampling_rate);

void livrar_residuais(type_effect *efeito);

void alocar_residuais(type_effect *efeito, long int sampling_rate);

void escrever_preset(type_effect efeitoA, type_effect efeitoB, type_effect efeitoC, FILE *file);

void carregar_preset(type_effect *efeitoA, type_effect *efeitoB, type_effect *efeitoC, FILE *file);

#endif
