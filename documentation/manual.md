Este documento traz uma breve sinopse de cada uma das funções e métodos disponíveis nas Bibliotecas criadas para este projeto, além de macros e tipos declarados nos arquivos cabeçalho.

# efeitos.h / efeitos.c

Sinais são interpretados como vetores de tipo `float`, todas as funções vão exigir no mínimo um ponteiro para entrada e outro para saída, podendo ser os dois o mesmo ponteiro se desejado um processamento destrutivo (sobrescrita).

`buffer` é o efeito padrão dos presets, escreve para `output` exatamente o que recebe em `sinal`. `LENGTH` é o comprimento dos vetores.

```C
void buffer(float *sinal, float *output, int LENGTH);
```

`amplificador_soft` escreve para `output` uma versão escalonada de `sinal` com ganho `GANHO` usando uma curva de transferência tangente hiperbólica, adicionando não linearidade (ganhos menores quanto maior a amplitude de entrada). `LENGTH` é o comprimento dos vetores.

```C
void amplificador_soft(float *sinal, float *output, int LENGTH, float GANHO);
```

`amplificador_hard` escreve para `output` uma versão escalonada com ganho `GANHO` de `sinal` usando uma curva de transferência linear, sem distorção. `LENGTH` é o comprimento dos vetores.

```C
void amplificador_hard(float *sinal, float *output, int LENGTH, float GANHO);
```

`saturador_soft` escreve para `output` uma versão distorcida com ganho `GANHO` de `sinal` usando uma curva de transferência tangente hiperbólica, o parâmetro `JANELA` é uma porcentagem do canal e quanto mais baixa, menores as amplitudes necessárias para saturar, emulando um amplificador valvulado ou gravação analógicas. `LENGTH` é o comprimento dos vetores.

```C
void saturador_soft(float *sinal, float *output, int LENGTH, float JANELA, float GANHO);
```

`saturador_hard` escreve para `output` uma versão distorcida com ganho `GANHO` de `sinal` usando uma curva de transferência linear, o parâmetro `JANELA` é uma porcentagem do canal e quanto mais baixa, menores as amplitudes necessárias para saturar. Similar à versão soft, mas o clipping é brusco e não há distorção de ganho próximo da saturação. `LENGTH` é o comprimento dos vetores.

```C
void saturador_hard(float *sinal, float *output, int LENGTH, float JANELA, float GANHO);
```

`echo` escreve para `output` uma cópia de `sinal` somada a cópias de si mesma, atrasadas em `DURATION` segundos, com um decaimento em progressão geométrica de fator `DECAY`. `LENGTH` é o comprimento dos vetores. Além disso é necessário conhecer a taxa de amostragem `sampling_rate` do sistema para converter tempo contínuo em número de amostras. `WET` é uma porcentagem interpoladora: 0 significa um buffer (sem efeito) e 1 aplicação total do efeito. Necessita de um vetor para memória residual `y_residual` para funcionamento. `LENGTH` é o comprimento dos vetores. Além disso como a janela de processamento é pequena em comparação ao possível tamanho do atraso entre as cópias, esse efeito precisa de um `indexador_externo` como iterador, garantindo que o processamento seja continuado de um pacote para o próximo.

```C
void echo(float *sinal, float *output, float *y_residual, long int *indexador_externo, int LENGTH, float DURATION, long int sampling_rate, float DECAY, float WET);
```

`lowpass` escreve para `output` uma cópia de `sinal` filtrada de suas componentes de alta frequência. A frequência em Hz a partir da qual a atenuação é mais intensa é `ANALOG_FREQUENCY`. Necessita de um vetor para memória residual `y_residual` para funcionamento. `LENGTH` é o comprimento dos vetores. `WET` é uma porcentagem interpoladora: 0 significa um buffer (sem efeito) e 1 aplicação total do efeito. É necessário conhecer a taxa de amostragem `sampling_rate` do sistema.

```C
void lowpass(float *sinal, float *output, float *y_residual, long int sampling_rate, int LENGTH, float ANALOG_FREQUENCY, float WET);
```

`highpass` escreve para `output` uma cópia de `sinal` filtrada de suas componentes de baixa frequência. A frequência em Hz até a qual atenuação é mais intensa é `ANALOG_FREQUENCY`. Necessita de dois vetores para memória residual `y_residual` e `x_residual` para funcionamento. `LENGTH` é o comprimento dos vetores. `WET` é uma porcentagem interpoladora: 0 significa um buffer (sem efeito) e 1 aplicação total do efeito. É necessário conhecer a taxa de amostragem `sampling_rate` do sistema.

```C
void highpass(float *sinal, float *output, float *x_residual, float *y_residual, long int sampling_rate, int LENGTH, float ANALOG_FREQUENCY, float WET);
```

`notch` escreve para `output` uma cópia de `sinal` filtrada de uma faixa de frequências bem estreita. A frequência em Hz central dessa faixa é `ANALOG_FREQUENCY`. Necessita de dois vetores para memória residual `y_residual` e `x_residual` para funcionamento. `LENGTH` é o comprimento dos vetores. `WET` é uma porcentagem interpoladora: 0 significa um buffer (sem efeito) e 1 aplicação total do efeito. É necessário conhecer a taxa de amostragem `sampling_rate` do sistema. Esse efeito precisa de um `indexador_externo` como iterador, garantindo que o processamento seja continuado de um pacote para o próximo.

```C
void notch(float *sinal, float *output, float *x_residual, float *y_residual, long int *indexador_externo, long int sampling_rate, int LENGTH, float ANALOG_FREQUENCY, float WET);
```

# interface.h / efeitos.c

Efeitos, enquanto estão amortecidos no código são compostos de um caractere `identificador`, um vetor `parametro` de números `float`. Além disso precisam de ponteiros para valores residuais x, y de ambos canais e iteradores para serem usados por efeitos com memória (que precisam ter resíduos do que acontece em um pacote de processamento para os próximos).

```C
#define NUMBER_OF_PARAMETERS 4

typedef struct{

    char identificador;
    float parametro[NUMBER_OF_PARAMETERS];

    float *x_residual_esquerdo;
    float *y_residual_esquerdo;

    float *x_residual_direito;
    float *y_residual_direito;

    long int indexador_modular_cumulativo_esquerdo;
    long int indexador_modular_cumulativo_direito;

}type_effect;
```

O `identificador` do efeito pode ser um destes.

```C
#define BUFFER      0x0 
#define SOFT_AMP    0x1 // [0] GANHO
#define HARD_AMP    0x2 // [0] GANHO
#define SOFT_SAT    0x3 // [0] GANHO    [1] JANELA
#define HARD_SAT    0x4 // [0] GANHO    [1] JANELA
#define ECHO        0x5 // [0] WET      [1] DURATION    [2] DECAY
#define LOWPASS     0x6 // [0] WET      [1] CUTOFF
#define HIGHPASS    0x7 // [0] WET      [1] CUTOFF
#define NOTCH       0x8 // [0] WET      [1] CUTOUT
```

`inicializar_efeito` inicializa toda a memória interna do efeito, variáveis para 0 e ponteiros para `NULL`. Toda instância precisa ser inicializada.

```C
void inicializar_efeito(type_effect *efeito);
```

`alocar_residuais` aloca dinamicamente as memórias residuais que o efeito precisa de acordo com seu identificador. É preciso conhecer a `samplig_rate` do sistema para efeitos que são baseados em tempo.

```C
void alocar_residuais(type_effect *efeito, long int sampling_rate);
```

`livrar_residuais` desaloca as memórias que foram dinamicamente alocadas para o efeito.

```C
void livrar_residuais(type_effect *efeito);
```

`imprime_efeito` escreve para o emulador de terminal os dados de ume efeito.

```C
void imprime_efeito(type_effect *efeito);
```

`switch_process` é uma forma autônoma de aplicar um efeito a um vetor, não há necessidade de chamadas manuais das funções de `efeitos.c`, basta passar os vetores de entrada e saída de ambos canais, além de um ponteiro para `type_effect` adicional contendo um efeito já inicializado e configurado. De acordo com o identificador `switch_process` consegue processar os vetores de acordo com o que se deseja. `signal_length` é o tamanho dos vetores e é preciso conhecer a `samplig_rate` do sistema.

```C
void switch_process(type_effect *efeito, float *entrada_esquerdo, float *entrada_direito, float *saida_esquerdo, float *saida_direito, long int signal_length, long int sampling_rate);
```

`escrever_preset` e  `carregar_preset` são usadas para escrever e ler presets (conjuntos de 3 efeitos) para arquivos. Elas recebem 3 efeitos e um ponteiro para arquivo, que deve ter sido inicialmente aberto em modo de leitura ou escrita de acordo com qual operação se realiza no momento.

```C
void escrever_preset(type_effect efeitoA, type_effect efeitoB, type_effect efeitoC, FILE *file);

void carregar_preset(type_effect *efeitoA, type_effect *efeitoB, type_effect *efeitoC, FILE *file);
```

# status.h / status.cpp

O núcleo da Interface de Usuário é uma máquina de estados finitos, onde o estado é um dos seguintes:

```C++
enum SCREEN_STATE 
{ 
  PRESET_SELECT,
  PRESET_EDIT,
  PRESET_DEL,
  HUB_MENU,
  PROCESSING,
  FX_EDIT,
  FX_ID_CHANGE,
  FX_PARAM_EDIT
};
```

Essa máquina é uma instância da classe `Status`. Toda informação relevante para as trocas de estado acontecerem de forma correta são atributos privados dessa classe. O fluxo de trabalho no ponto de vista do código principal é o seguinte:

A máquina precise ser instanciada com `Status::Status` passados como parâmetros o GPIO dos pinos físicos para botão A, botão B, e os pinos do encoder: Data, Clock e Switch.

```C++
Status::Status(int pin_but_a, int pin_but_b, int pin_data, int pin_clk, int pin_encoder_sw)
{
  // acesso aos pinos físios
  PIN_BUT_A =       pin_but_a;
  PIN_BUT_B =       pin_but_b;
  PIN_ENCODER_SW =  pin_encoder_sw;

  PIN_CLK =         pin_clk;
  PIN_DATA =        pin_data;

  // inicialização dos atributos privados
  INTERFACE_STATE = PRESET_SELECT;

  INTERFACE_CURSOR_POSITION = 0;
  PAGINA_VIRTUAL = 0;

  AMOUNT_PRESETS = readFolderPresetAmount();

  ACTIVE_PRESET_FILE = 0;
  ACTIVE_EFFECT = 0;
  ACTIVE_PARAM = 0;
  EDIT_AUX = 0;
  STEP_SIZE = 0;

  // suporte ao encoder rotativo
  ENCODER_PULSE_COUNTER = 0;

  // flag que sobe se alguma atividade relevante ocorrer
  ACTIVITY = 0;

  // alocação dinâmica dos efeitos para o buffer preset
  int j;
  for(j=0; j<3; j++)
  {
    PRESET[j] = new Efeito();
  }
}
```

Com a instância criada precisamos configurar dois `Interrupt Handlers` para cada canal do encoder rotacional. Um exemplo desses Handlers para Arduino UNO R3:

```C++
attachInterrupt(digitalPinToInterrupt(ENCODER_CLK),   HANDLER_CLK, CHANGE);
attachInterrupt(digitalPinToInterrupt(ENCODER_DATA),  HANDLER_DATA, CHANGE);
```

```C++
void HANDLER_CLK()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();

  if (interrupt_time - last_interrupt_time > ENCODER_DEBOUNCE) 
  {
    if(bitRead(PIND,ENCODER_DATA) != bitRead(PIND,ENCODER_CLK)) 
    {
      status.incrementarEncoderPulseCounter();
    } 
    else 
    {
      status.decrementarEncoderPulseCounter();
    }
  }
  last_interrupt_time = interrupt_time;
}
```

```C++
void HANDLER_DATA()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();

  if (interrupt_time - last_interrupt_time > ENCODER_DEBOUNCE) 
  {
    if (bitRead(PIND,ENCODER_CLK) == bitRead(PIND,ENCODER_DATA)) 
    {
      status.incrementarEncoderPulseCounter();
    } 
    else 
    {
      status.decrementarEncoderPulseCounter();
    }
  }
  last_interrupt_time = interrupt_time;
}
```

É essencial que esses Handlers chamem as funções `decrementarEncoderPulseCounter` e `incrementarEncoderPulseCounter`. É com essas chamadas que a Máquina de Estados pode receber inputs rotacionais do encoder.

```C++
// manipulação do encoder, chamada no handler da interrupção
void  decrementarEncoderPulseCounter();
void  incrementarEncoderPulseCounter();
```

O restante dos inputs (apertos de botão) são gerenciados sem interrupções, basta chamar `updateStatus()` uma vez a cada laço de repetição do código principal.

```C++
void  updateStatus();
```

Uma vez por ciclo do laço de repetição também deve ser invocada `getActivity()` que checa uma flag interna que registra algum acontecimento relevante o suficinete para valer alterar o display (um processo lento).

```C++
int   getActivity();
```

As seguintes funções podem ser chamadas no código principal para recuperar atributos privados da classe que podem ser necessários para desenhar informações para o display:

```C++
// ler os atributos privados da máquina de estados --------------------------------
SCREEN_STATE  getInterfaceState();

int           getCursorPosition();
int           getPaginaVirtual();

int           getActivePreset();
int           getActiveEffect();
int           getActiveParam();

// usadas para desenhar na tela os valores enquanto estão sendo editados no buffer
Efeito**      getPresetBuffer();
float         getEditAux();
```

Por fim, as funções públicas para escrita ou leitura de arquivos ainda precisam de implementação na Integração final.

```C++
// IO de arquivo (vão precisar de um ponteiro para os arquivos de preset)
void          burnFileToBuffer(); // precisa de um ponteiro para arquivo aberto em leitura
void          burnBufferToFile(); // precisa de um ponteiro para arquivo aberto em escrita

// Leitura da folder de presets pra contar quantos já existem na inicialização
int          readFolderPresetAmount();
```

# Referências Externas

O repositório com o Código Fonte integral pode ser acessado [aqui](https://github.com/Nilton-Miguel/ELE64/tree/main)
