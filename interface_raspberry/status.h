#ifndef STATUS_h
#define STATUS_h

//#include <Arduino.h>
#include <wiringPi.h>
#include <cstdio>
#include <cstdlib>
#include <string>

#define EXISTING_PRESETS            2

#define ENCODER_DEBOUNCE            5
#define ENCODER_COUNTER_THRESHOLD   5

#define BUT_A         8
#define BUT_B         9
#define ENCODER_SW    6
#define ENCODER_DATA  5
#define ENCODER_CLK   4

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

// tabelar os IDs de novo
#define MACRO_BUFFER        0x0
#define MACRO_SOFT_AMP      0x1
#define MACRO_HARD_AMP      0x2
#define MACRO_SOFT_SAT      0x3
#define MACRO_HARD_SAT      0x4
#define MACRO_ECHO          0x5
#define MACRO_LOWPASS       0x6
#define MACRO_HIGHPASS      0x7
#define MACRO_NOTCH         0x8

// ---------------------------------------------------------------------------------
class Efeito
{
private:

  char ID;
  float parametro[4];

public:

  Efeito();

  void  alterarID(char novo_ID);
  void  alterarParametro(int indice, float novo_valor);

  char  recuperarID();
  float recuperarParametro(int indice);
};
// ---------------------------------------------------------------------------------
class StatusClass
{
private:

  // user input output information
  SCREEN_STATE INTERFACE_STATE;

  int INTERFACE_CURSOR_POSITION;
  int PAGINA_VIRTUAL;

  int ACTIVE_PRESET_FILE;
  int ACTIVE_EFFECT;
  int ACTIVE_PARAM;

  float         EDIT_AUX;
  int           STEP_SIZE;
  volatile int  ENCODER_PULSE_COUNTER;
  int           ACTIVITY;

  // amounts
  int       AMOUNT_PRESETS;
  const int AMOUNT_FX = 9;

  // physical inputs we monitor
  int PIN_BUT_A;
  int PIN_BUT_B;
  int PIN_ENCODER_SW;
  int PIN_DATA;
  int PIN_CLK;

  // old values from physcal inputs
  int OLD_STATUS_BUT_A;
  int OLD_STATUS_BUT_B;
  int OLD_STATUS_ENCODER_SW;

  // new values from physical inputs
  int NEW_STATUS_BUT_A;
  int NEW_STATUS_BUT_B;
  int NEW_STATUS_ENCODER_SW;

  // effect buffer
  Efeito *PRESET[3];

  //-------------------------------------------------------------------------
  // funções que só têm aplicação interna à classe
  //-------------------------------------------------------------------------
  void  inputHandler();

  void  zerarPreset();
  void  zerarEfeito(int indice);

  int   getDifferenceButA();
  int   getDifferenceButB();
  int   getDifferenceEncoderSwitch();

  int   getCursorUp();
  int   getCursorDown();

  void startProcessing();
  void stopProcessing();
  //-------------------------------------------------------------------------

public:

  // ---------------------------------------------------------------------------------
  // geradora de instância
  StatusClass(int pin_but_a, int pin_but_b, int pin_data, int pin_clk, int pin_encoder_sw);

  // precisa ser chamada todo ciclo do loop externo
  void  updateStatus();

  // manipulação do encoder, chamada no handler da interrupção
  void  decrementarEncoderPulseCounter();
  void  incrementarEncoderPulseCounter();

  // flag que monitora quando é relevante atualizar a tela
  int   getActivity();

  // ler os atributos privados da máquina de estados --------------------------------
  SCREEN_STATE  getInterfaceState();

  int           getCursorPosition();
  int           getPaginaVirtual();
  int           getOption();

  int           getAmountPresets();
  int           getActivePreset();
  int           getActiveEffect();
  int           getActiveParam();

  // usadas para desenhar na tela os valores enquanto estão sendo editados no buffer
  Efeito**      getPresetBuffer();
  float         getEditAux();

  // FUNÇÕES QUE AINDA PRECISAM DE IMPLEMENTAÇÃO (JOÃO) -----------------------------

  // IO de arquivo (vão precisar de um ponteiro para os arquivos de preset)
  void          burnFileToBuffer(); // precisa de um ponteiro para arquivo aberto em leitura
  void          burnBufferToFile(); // precisa de um ponteiro para arquivo aberto em escrita

  // Leitura da folder de presets pra contar quantos já existem na inicialização
  int          readFolderPresetAmount();
  // ---------------------------------------------------------------------------------
};

#endif
