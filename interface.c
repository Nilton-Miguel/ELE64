
#include "interface.h"
#include "efeitos.h"

#include <stdlib.h>
#include <stdio.h>

void inicializar_efeito(type_effect *efeito)
{
    efeito -> identificador = 0x0;
    int j;
    for(j=0; j<NUMBER_OF_PARAMETERS; j++) efeito -> parametro[j] = 0.0;

    efeito -> x_residual_esquerdo = NULL;
    efeito -> y_residual_esquerdo = NULL;

    efeito -> x_residual_direito = NULL;
    efeito -> y_residual_direito = NULL;
}
void imprime_efeito(type_effect *efeito)
{
    printf("identificador: 0x%d ", efeito -> identificador);
    printf("parametros: ");
    int j;
    for(j=0; j<NUMBER_OF_PARAMETERS; j++) printf("%f ", efeito -> parametro[j]);
    
    printf("\n");
}
void switch_process(type_effect *efeito, float *entrada_esquerdo, float *entrada_direito, float *saida_esquerdo, float *saida_direito, long int signal_length, long int sampling_rate)
{
    switch (efeito -> identificador)
    {
        case BUFFER:

            buffer(entrada_esquerdo, saida_esquerdo, signal_length);
            buffer(entrada_direito,  saida_direito,  signal_length);
            break;

        case SOFT_AMP:

            amplificador_soft(entrada_esquerdo, saida_esquerdo, signal_length, efeito -> parametro[0]);
            amplificador_soft(entrada_direito,  saida_direito,  signal_length, efeito -> parametro[0]);
            break;

        case HARD_AMP:

            amplificador_hard(entrada_esquerdo, saida_esquerdo, signal_length, efeito -> parametro[0]);
            amplificador_hard(entrada_direito,  saida_direito,  signal_length, efeito -> parametro[0]);
            break;

        case SOFT_SAT:

            saturador_soft(entrada_esquerdo, saida_esquerdo, signal_length, efeito -> parametro[1], efeito -> parametro[0]);
            saturador_soft(entrada_direito,  saida_direito,  signal_length, efeito -> parametro[1], efeito -> parametro[0]);
            break;

        case HARD_SAT:

            saturador_hard(entrada_esquerdo, saida_esquerdo, signal_length, efeito -> parametro[1], efeito -> parametro[0]);
            saturador_hard(entrada_direito,  saida_direito,  signal_length, efeito -> parametro[1], efeito -> parametro[0]);
            break;

        case ECHO:

            echo(entrada_esquerdo, saida_esquerdo, efeito -> y_residual_esquerdo, signal_length, efeito -> parametro[1], efeito -> parametro[2], efeito-> parametro[0]);
            echo(entrada_direito, saida_direito, efeito -> y_residual_direito, signal_length, efeito -> parametro[1], efeito -> parametro[2], efeito-> parametro[0]);
            break;

        case LOWPASS:

            lowpass(entrada_esquerdo, saida_esquerdo, efeito->y_residual_esquerdo, sampling_rate, signal_length, efeito->parametro[1], efeito->parametro[0]);
            lowpass(entrada_direito, saida_direito, efeito->y_residual_direito, sampling_rate, signal_length, efeito->parametro[1], efeito->parametro[0]);
            break;
        
        case HIGHPASS:

            highpass(entrada_esquerdo, saida_esquerdo, efeito->x_residual_esquerdo, efeito->y_residual_esquerdo, sampling_rate, signal_length, efeito->parametro[1], efeito->parametro[0]);
            highpass(entrada_direito, saida_direito, efeito->x_residual_direito, efeito->y_residual_direito, sampling_rate, signal_length, efeito->parametro[1], efeito->parametro[0]);
            break;
        
        case NOTCH:

            notch(entrada_esquerdo, saida_esquerdo, efeito->x_residual_esquerdo, efeito->y_residual_esquerdo, sampling_rate, signal_length, efeito->parametro[1], efeito->parametro[0]);
            notch(entrada_direito, saida_direito, efeito->x_residual_direito, efeito->y_residual_direito, sampling_rate, signal_length, efeito->parametro[1], efeito->parametro[0]);
            break;
        
        default:

            break;
    }
}
void livrar_residuais(type_effect *efeito)
{
    free(efeito -> x_residual_esquerdo);
    free(efeito -> y_residual_esquerdo);
    free(efeito -> x_residual_direito);
    free(efeito -> y_residual_direito);
}
void alocar_residuais(type_effect *efeito)
{
    switch (efeito -> identificador)
    {
    case ECHO:

        // memoria residual
        // x : 0
        // y : DURATION
        
        // alocar
        efeito -> y_residual_esquerdo = (float*)malloc((efeito->parametro[1])*sizeof(float));
        efeito -> y_residual_direito = (float*)malloc((efeito->parametro[1])*sizeof(float));

        // inicializar
        long int j;
        for(j=0; j<efeito->parametro[1]; j++)
        {
            efeito -> y_residual_esquerdo[j] = 0;
            efeito -> y_residual_direito[j] = 0;
        }

        break;
    
    case LOWPASS:

        // memoria residual
        // x : 0
        // y : 1
        
        // alocar
        efeito -> y_residual_esquerdo = (float*)malloc(sizeof(float));
        efeito -> y_residual_direito =  (float*)malloc(sizeof(float));

        // inicializar
        efeito -> y_residual_esquerdo[0] = 0;
        efeito -> y_residual_direito[0] = 0;

        break;
    
    case HIGHPASS:

        // memoria residual
        // x : 1
        // y : 1

        // alocar
        efeito -> x_residual_esquerdo = (float*)malloc(sizeof(float));
        efeito -> y_residual_esquerdo = (float*)malloc(sizeof(float));
        efeito -> x_residual_direito =  (float*)malloc(sizeof(float));
        efeito -> y_residual_direito =  (float*)malloc(sizeof(float));

        // inicializar
        efeito -> x_residual_esquerdo[0] = 0;
        efeito -> y_residual_esquerdo[0] = 0;
        efeito -> x_residual_direito[0] = 0;
        efeito -> y_residual_direito[0] = 0;

        break;

    case NOTCH:

        // memoria residual
        // x : 2
        // y : 2

        efeito -> x_residual_esquerdo = (float*)malloc(2*sizeof(float));
        efeito -> y_residual_esquerdo = (float*)malloc(2*sizeof(float));

        efeito -> x_residual_direito =  (float*)malloc(2*sizeof(float));
        efeito -> y_residual_direito =  (float*)malloc(2*sizeof(float));
        break;

    default:

        break;
    }
}
