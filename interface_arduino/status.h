#ifndef STATUS_h
#define STATUS_h

#include <Arduino.h>

#define EXISTING_PRESETS            12

#define ENCODER_DEBOUNCE            5
#define ENCODER_COUNTER_THRESHOLD   5

#define BUT_A         7
#define BUT_B         6
#define CURSOR        4
#define ENCODER_SW    5
#define ENCODER_DATA  3
#define ENCODER_CLK   2

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

class Status
{
private:

  // user input output information
  SCREEN_STATE INTERFACE_STATE;

  int INTERFACE_CURSOR_POSITION;
  int PAGINA_VIRTUAL;

  int ACTIVE_PRESET_FILE;
  int ACTIVE_EFFECT;
  int ACTIVE_PARAM;

  float EDIT_AUX;
  int STEP_SIZE;
  volatile int ENCODER_PULSE_COUNTER;

  /*
    INTERFACE_STATE             - qual o estado atual da maquina
    INTERFACE_CURSOR_POSITION   - qual dos elementos da tela está emhighlight no momento
    PAGINA_VIRTUAL              - qual pagina está sendo mostrada (para telas multi pagina)
    ACTIVE_PRESET_FILE          - qual o indice do arquivo preset sendo lido ou editado
    ACTIVE_EFFECT               - qual dos 3 efeitos está sendo editado
    ACTIVE_PARAM                - qual dos 4 parametros está sendo editado
    EDIT_AUX                    - buffer para guardar as alterações no parametro antes de serem efetivadas
    STEP_SIZE                   - estado que guarda se queremos passos pequenos ou grandes
  */

  // amounts
  int AMOUNT_PRESETS;
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

public:

  Status(int pin_but_a, int pin_but_b, int pin_data, int pin_clk, int pin_encoder_sw);

  void  printStatus();
  void  printEfeito(int indice);

  void  zerarPreset();
  void  zerarEfeito(int indice);

  void  updateStatus();
  void  inputHandler();

  int   getDifferenceButA();
  int   getDifferenceButB();
  int   getDifferenceEncoderSwitch();

  int   decrementarEncoderPulseCounter();
  int   incrementarEncoderPulseCounter();
  int   getCursorUp();
  int   getCursorDown();
};
// ---------------------------------------------------------------------------------

#endif
