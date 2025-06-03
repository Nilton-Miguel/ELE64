
#include "interface.h"

#include <stdlib.h>
#include <stdio.h>

int main(){

    type_effect efeitoA, efeitoB, efeitoC;

    inicializar_efeito(&efeitoA); 
    inicializar_efeito(&efeitoB); 
    inicializar_efeito(&efeitoC);

    imprime_efeito(&efeitoA);
    imprime_efeito(&efeitoB);
    imprime_efeito(&efeitoC);

    return 0;
}
