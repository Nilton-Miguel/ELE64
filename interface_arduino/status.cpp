
#include "status.h"

// ---------------------------------------------------------------------------------
Efeito::Efeito()
{
  ID = 0x0;

  int j;
  for(j=0; j<4; j++) parametro[j] = 0;
}
// ---------------------------------------------------------------------------------

void Efeito::alterarID(char novo_ID)
{
  ID = novo_ID;
}

void Efeito::alterarParametro(int indice, float novo_valor)
{
  parametro[indice] = novo_valor;
}

char Efeito::recuperarID()
{
  return ID;
}

float Efeito::recuperarParametro(int indice)
{
  return parametro[indice];
}

// ---------------------------------------------------------------------------------
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

  AMOUNT_PRESETS = EXISTING_PRESETS;

  ACTIVE_PRESET_FILE = 0;
  ACTIVE_EFFECT = 0;
  ACTIVE_PARAM = 0;
  EDIT_AUX = 0;
  STEP_SIZE = 0;

  // suporte ao encoder rotativo
  ENCODER_PULSE_COUNTER = 0;

  // alocação dinâmica dos efeitos para o buffer preset
  int j;
  for(j=0; j<3; j++)
  {
    PRESET[j] = new Efeito();
  }
}
// ---------------------------------------------------------------------------------

void Status::printStatus()
{

  // atributos internos ----------------------------------

  Serial.print("|ESTADO| 0x");
  Serial.print(INTERFACE_STATE);

  //Serial.print(" BUT_A: ");
  //Serial.print(NEW_STATUS_BUT_A);

  //Serial.print(" BUT_B: ");
  //Serial.print(NEW_STATUS_BUT_B);

  //Serial.print(" DIFFA: ");
  //Serial.print(getDifferenceButA());

  //Serial.print(" DIFFB: ");
  //Serial.print(getDifferenceButB());

  Serial.print(" |CURSOR| 0x");
  Serial.print(INTERFACE_CURSOR_POSITION);

  Serial.print(" |VPAGE| 0x");
  Serial.print(PAGINA_VIRTUAL);

  Serial.print(" |PRESET| 0x");
  Serial.print(ACTIVE_PRESET_FILE);

  Serial.print(" |FX| 0x");
  Serial.print(ACTIVE_EFFECT);

  //Serial.print(" |ENCODER COUNTER| ");
  //Serial.print(ENCODER_PULSE_COUNTER);

  // eventos --------------------------------------------

  if(getDifferenceButA() && NEW_STATUS_BUT_A == 0)                      Serial.print("|A PRESSED|");
  else if(getDifferenceButB() && NEW_STATUS_BUT_B == 0)                 Serial.print("|B PRESSED|");
  else if(getDifferenceEncoderSwitch() && NEW_STATUS_ENCODER_SW == 1)   Serial.print("|ENC PRESSED|");
  else if(getCursorUp())                                                Serial.print("|CURSOR UP|");
  else if(getCursorDown())                                              Serial.print("|CURSOR DOWN|");

  Serial.println();
}

void  Status::printEfeito(int indice)
{

  Serial.print("FX ");
  Serial.print(indice);

  Serial.print(" |AFX| 0x");
  Serial.print(ACTIVE_EFFECT);

  if((0 == INTERFACE_CURSOR_POSITION) && INTERFACE_STATE == FX_EDIT)
    {
      Serial.print(" >> ");
    }
  Serial.print(" ID: ");
  Serial.print((int)(PRESET[indice] -> recuperarID()));
  Serial.print(" PR : ");

  int j;
  for(j=0; j<4; j++)
  {
    if((j + 1 == INTERFACE_CURSOR_POSITION) && INTERFACE_STATE == FX_EDIT)
    {
      Serial.print(" >> ");
    }
    Serial.print(PRESET[indice] -> recuperarParametro(j));
    Serial.print("   ");
  }

  Serial.print(" |ST| 0x");
  Serial.print(INTERFACE_STATE);

  Serial.print("|OPTION| 0x");
  Serial.print(5*PAGINA_VIRTUAL + INTERFACE_CURSOR_POSITION);

  Serial.print(" |PR| 0x");
  Serial.print(ACTIVE_PARAM);

  Serial.print(" |AUX| ");
  Serial.print(EDIT_AUX);

  Serial.println();
}

void Status::zerarPreset()
{
  int i, j;
  for(i=0; i<3; i++)
  {
    PRESET[i] -> alterarID(0);
    for(j=0; j<4; j++)
    {
      PRESET[i] -> alterarParametro(j, 0);
    }
  }
}

void Status::zerarEfeito(int indice)
{
  
  PRESET[indice] -> alterarID(0);

  int j;
  for(j=0; j<4; j++)
  {
    PRESET[indice] -> alterarParametro(j, 0);
  }
}

void Status::updateStatus()
{
  OLD_STATUS_BUT_A =      NEW_STATUS_BUT_A;
  OLD_STATUS_BUT_B =      NEW_STATUS_BUT_B;
  OLD_STATUS_ENCODER_SW = NEW_STATUS_ENCODER_SW;

  NEW_STATUS_BUT_A =      digitalRead(PIN_BUT_A);
  NEW_STATUS_BUT_B =      digitalRead(PIN_BUT_B);
  NEW_STATUS_ENCODER_SW = digitalRead(PIN_ENCODER_SW);

  inputHandler();
}

void Status::inputHandler()
{
  if(getDifferenceButA() && NEW_STATUS_BUT_A == 0)
  {
    switch(INTERFACE_STATE)
    {
      case PRESET_SELECT:

        switch(5*PAGINA_VIRTUAL + INTERFACE_CURSOR_POSITION)
        {
          case 0:
            // cria o novo preset
            AMOUNT_PRESETS++;
            ACTIVE_PRESET_FILE = AMOUNT_PRESETS;

            // troca o estado
            INTERFACE_STATE = PRESET_EDIT;
          break;

          default:
            // seleciona o preset ja existente
            ACTIVE_PRESET_FILE = 5*PAGINA_VIRTUAL + INTERFACE_CURSOR_POSITION;

            // troca o estado
            INTERFACE_STATE = HUB_MENU;
        }
        // resetar o navegador
        INTERFACE_CURSOR_POSITION = 0;
        PAGINA_VIRTUAL = 0;
        break;

      case PRESET_EDIT:

        INTERFACE_STATE = FX_EDIT;
        ACTIVE_EFFECT = INTERFACE_CURSOR_POSITION;
        INTERFACE_CURSOR_POSITION = 0;
        PAGINA_VIRTUAL = 0;
        break;

      case PRESET_DEL: 
        break;

      case HUB_MENU:

        switch(INTERFACE_CURSOR_POSITION)
        {
          case 0:
            //processar
            INTERFACE_STATE = PROCESSING;
            INTERFACE_CURSOR_POSITION = 0;
            PAGINA_VIRTUAL = 0;
          break;
   
          case 1:
            //reset total
            zerarPreset();
            INTERFACE_STATE = PRESET_SELECT;
            INTERFACE_CURSOR_POSITION = 0;
            PAGINA_VIRTUAL = 0;
            ACTIVE_PRESET_FILE = 0;
          break;

          case 2:
            //editar o arquivo atual
            INTERFACE_STATE = PRESET_EDIT;
            INTERFACE_CURSOR_POSITION = 0;
            PAGINA_VIRTUAL = 0;
          break;

          case 3:
            //deletar o arquivo atual
            INTERFACE_STATE = PRESET_DEL;
            INTERFACE_CURSOR_POSITION = 0;
            PAGINA_VIRTUAL = 0;
          break;
        }
        break;

      case PROCESSING:

        INTERFACE_STATE = HUB_MENU;
        INTERFACE_CURSOR_POSITION = 0;
        PAGINA_VIRTUAL = 0;
        break;

      case FX_EDIT:

        switch(INTERFACE_CURSOR_POSITION)
        {
          case 0:

            // troca do ID
            INTERFACE_STATE = FX_ID_CHANGE;
          break;
          default:

            // alteração de algum dos parâmetros
            INTERFACE_STATE = FX_PARAM_EDIT;
            ACTIVE_PARAM = INTERFACE_CURSOR_POSITION - 1;

            EDIT_AUX = PRESET[ACTIVE_EFFECT] -> recuperarParametro(ACTIVE_PARAM);
        }
        INTERFACE_CURSOR_POSITION = 0;
        PAGINA_VIRTUAL = 0;
        break;

      case FX_ID_CHANGE:

        zerarEfeito(ACTIVE_EFFECT);
        PRESET[ACTIVE_EFFECT] -> alterarID(5*PAGINA_VIRTUAL + INTERFACE_CURSOR_POSITION);
        
        INTERFACE_STATE = FX_EDIT;
        INTERFACE_CURSOR_POSITION = 0;
        PAGINA_VIRTUAL = 0;
        break;

      case FX_PARAM_EDIT:

        // registrar a alteração
        PRESET[ACTIVE_EFFECT] -> alterarParametro(ACTIVE_PARAM, EDIT_AUX);

        // voltar pro editor de efeito
        INTERFACE_STATE = FX_EDIT;
        INTERFACE_CURSOR_POSITION = 0;
        PAGINA_VIRTUAL = 0;
        ACTIVE_PARAM = 0;
        break;
    }
  }
  else if(getDifferenceButB() && NEW_STATUS_BUT_B == 0)
  {
    switch(INTERFACE_STATE)
    {
      case PRESET_SELECT: 
        break;

      case PRESET_EDIT:

        INTERFACE_STATE = HUB_MENU;
        INTERFACE_CURSOR_POSITION = 0;
        PAGINA_VIRTUAL = 0;
        break;

      case PRESET_DEL:

        INTERFACE_STATE = HUB_MENU;
        INTERFACE_CURSOR_POSITION = 0;
        PAGINA_VIRTUAL = 0;
        break;

      case HUB_MENU:

        //reset total
        zerarPreset();
        INTERFACE_STATE = PRESET_SELECT;
        INTERFACE_CURSOR_POSITION = 0;
        PAGINA_VIRTUAL = 0;
        ACTIVE_PRESET_FILE = 0;
        break;

      case PROCESSING:

        INTERFACE_STATE = PRESET_EDIT;
        INTERFACE_CURSOR_POSITION = 0;
        PAGINA_VIRTUAL = 0;
        break;

      case FX_EDIT:

        INTERFACE_STATE = PRESET_EDIT;
        INTERFACE_CURSOR_POSITION = 0;
        PAGINA_VIRTUAL = 0;
        ACTIVE_EFFECT = 0;
        break;

      case FX_ID_CHANGE:

        INTERFACE_STATE = FX_EDIT;
        INTERFACE_CURSOR_POSITION = 0;
        PAGINA_VIRTUAL = 0;
        break;

      case FX_PARAM_EDIT:

        // voltar pro editor de efeito sem registrar a alteração
        INTERFACE_STATE = FX_EDIT;
        INTERFACE_CURSOR_POSITION = 0;
        PAGINA_VIRTUAL = 0;
        ACTIVE_PARAM = 0;
        break;
    }
  }
  else if(getCursorUp())
  {
    // output da função age como uma flag que é baixada quando o input é atendido
    ENCODER_PULSE_COUNTER = 0;

    switch(INTERFACE_STATE)
    {
      case PRESET_SELECT:
        
        INTERFACE_CURSOR_POSITION++;

        if(5*PAGINA_VIRTUAL + INTERFACE_CURSOR_POSITION > AMOUNT_PRESETS)
        {
          // reset das paginas se ultrapassou o ultimo preset
          PAGINA_VIRTUAL = 0;
          INTERFACE_CURSOR_POSITION = 0;
        }
        else if(INTERFACE_CURSOR_POSITION == 5)
        {
          // troca de pagina se ultrapassou o limite dos 5 presets da pagina
          INTERFACE_CURSOR_POSITION -= 5; 
          PAGINA_VIRTUAL++;
        }
        break;

      case PRESET_EDIT:

        INTERFACE_CURSOR_POSITION++;
        INTERFACE_CURSOR_POSITION = INTERFACE_CURSOR_POSITION % 3;
        break;

      case PRESET_DEL: 
        break;

      case HUB_MENU:

        INTERFACE_CURSOR_POSITION++;
        INTERFACE_CURSOR_POSITION = INTERFACE_CURSOR_POSITION % 4;
        break;

      case PROCESSING: 
        break;

      case FX_EDIT:

        INTERFACE_CURSOR_POSITION++;
        INTERFACE_CURSOR_POSITION = INTERFACE_CURSOR_POSITION % 5;
        break;

      case FX_ID_CHANGE:

        INTERFACE_CURSOR_POSITION++;

        if(5*PAGINA_VIRTUAL + INTERFACE_CURSOR_POSITION > AMOUNT_FX)
        {
          // reset das paginas se ultrapassou o ultimo preset
          PAGINA_VIRTUAL = 0;
          INTERFACE_CURSOR_POSITION = 0;
        }
        else if(INTERFACE_CURSOR_POSITION == 5)
        {
          // troca de pagina se ultrapassou o limite dos 5 presets da pagina
          INTERFACE_CURSOR_POSITION -= 5; 
          PAGINA_VIRTUAL++;
        }
        break;

      case FX_PARAM_EDIT:

        // passo grande
        if(STEP_SIZE == 0)
        {
          switch(PRESET[ACTIVE_EFFECT] -> recuperarID())
          {
            case MACRO_BUFFER:
              break;

            case MACRO_SOFT_AMP:

              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX += 0.5;
                  if(EDIT_AUX > 10) EDIT_AUX = 0;
                  break;

                default:
                  break;
              }
              break;

            case MACRO_HARD_AMP:

              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX += 0.5;
                  if(EDIT_AUX > 10) EDIT_AUX = 0;
                  break;

                default:
                  break;
              }
              break;

            case MACRO_SOFT_SAT:

              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX += 0.5;
                  if(EDIT_AUX > 10) EDIT_AUX = 0;
                  break;

                case 1:
                  EDIT_AUX += 0.05;
                  if(EDIT_AUX > 1) EDIT_AUX = 0;
                  break;

                default:
                  break;
              }
              break;

            case MACRO_HARD_SAT:
              
              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX += 0.5;
                  if(EDIT_AUX > 10) EDIT_AUX = 0;
                  break;

                case 1:
                  EDIT_AUX += 0.05;
                  if(EDIT_AUX > 1) EDIT_AUX = 0;
                  break;

                default:
                  break;
              }
              break;

            case MACRO_ECHO:

              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX += 0.05;
                  if(EDIT_AUX > 1) EDIT_AUX = 0;
                  break;

                case 1:
                  EDIT_AUX += 0.05;
                  if(EDIT_AUX > 2) EDIT_AUX = 0;
                  break;
                
                case 2:
                  EDIT_AUX += 0.05;
                  if(EDIT_AUX > 1) EDIT_AUX = 0;
                  break;

                default:
                  break;
              }
              break;

            case MACRO_LOWPASS:

              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX += 0.05;
                  if(EDIT_AUX > 1) EDIT_AUX = 0;
                  break;

                case 1:
                  EDIT_AUX += 500;
                  if(EDIT_AUX > 20000) EDIT_AUX = 0;
                  break;

                default:
                  break;
              }
              break;

            case MACRO_HIGHPASS:

              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX += 0.05;
                  if(EDIT_AUX > 1) EDIT_AUX = 0;
                  break;

                case 1:
                  EDIT_AUX += 500;
                  if(EDIT_AUX > 20000) EDIT_AUX = 0;
                  break;

                default:
                  break;
              }
              break;

            case MACRO_NOTCH:

              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX += 0.05;
                  if(EDIT_AUX > 1) EDIT_AUX = 0;
                  break;

                case 1:
                  EDIT_AUX += 500;
                  if(EDIT_AUX > 20000) EDIT_AUX = 0;
                  break;

                default:
                  break;
              }
              break;
          }
        }
        // passo pequeno
        else
        {
          switch(PRESET[ACTIVE_EFFECT] -> recuperarID())
          {
            case MACRO_BUFFER:
              break;

            case MACRO_SOFT_AMP:

              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX += 0.1;
                  if(EDIT_AUX > 10) EDIT_AUX = 0;
                  break;

                default:
                  break;
              }
              break;

            case MACRO_HARD_AMP:

              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX += 0.1;
                  if(EDIT_AUX > 10) EDIT_AUX = 0;
                  break;

                default:
                  break;
              }
              break;

            case MACRO_SOFT_SAT:

              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX += 0.1;
                  if(EDIT_AUX > 10) EDIT_AUX = 0;
                  break;

                case 1:
                  EDIT_AUX += 0.01;
                  if(EDIT_AUX > 1) EDIT_AUX = 0;
                  break;

                default:
                  break;
              }
              break;

            case MACRO_HARD_SAT:

              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX += 0.5;
                  if(EDIT_AUX > 10) EDIT_AUX = 0;
                  break;

                case 1:
                  EDIT_AUX += 0.05;
                  if(EDIT_AUX > 1) EDIT_AUX = 0;
                  break;

                default:
                  break;
              }
              break;

            case MACRO_ECHO:

              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX += 0.01;
                  if(EDIT_AUX > 1) EDIT_AUX = 0;
                  break;

                case 1:
                  EDIT_AUX += 0.01;
                  if(EDIT_AUX > 2) EDIT_AUX = 0;
                  break;
                
                case 2:
                  EDIT_AUX += 0.01;
                  if(EDIT_AUX > 1) EDIT_AUX = 0;
                  break;

                default:
                  break;
              }
              break;

            case MACRO_LOWPASS:

              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX += 0.01;
                  if(EDIT_AUX > 1) EDIT_AUX = 0;
                  break;

                case 1:
                  EDIT_AUX += 10;
                  if(EDIT_AUX > 20000) EDIT_AUX = 0;
                  break;

                default:
                  break;
              }
              break;

            case MACRO_HIGHPASS:

              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX += 0.01;
                  if(EDIT_AUX > 1) EDIT_AUX = 0;
                  break;

                case 1:
                  EDIT_AUX += 10;
                  if(EDIT_AUX > 20000) EDIT_AUX = 0;
                  break;

                default:
                  break;
              }
              break;

            case MACRO_NOTCH:

              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX += 0.01;
                  if(EDIT_AUX > 1) EDIT_AUX = 0;
                  break;

                case 1:
                  EDIT_AUX += 10;
                  if(EDIT_AUX > 20000) EDIT_AUX = 0;
                  break;

                default:
                  break;
              }
              break;
          }
        }
        break;
    }
  }
  else if(getCursorDown())
  {
    // output da função age como uma flag que é baixada quando o input é atendido
    ENCODER_PULSE_COUNTER = 0;

    switch(INTERFACE_STATE)
    {
      case PRESET_SELECT:
        
        INTERFACE_CURSOR_POSITION--;

        if(PAGINA_VIRTUAL <= 0 && INTERFACE_CURSOR_POSITION == -1)
        {
          PAGINA_VIRTUAL = AMOUNT_PRESETS/5;
          INTERFACE_CURSOR_POSITION = AMOUNT_PRESETS % 5;
        }
        else if (INTERFACE_CURSOR_POSITION == -1)
        {
          PAGINA_VIRTUAL--;
          INTERFACE_CURSOR_POSITION = 4;
        }
        break;

      case PRESET_EDIT:

        INTERFACE_CURSOR_POSITION--;
        if(INTERFACE_CURSOR_POSITION < 0) INTERFACE_CURSOR_POSITION = 2;
        break;

      case PRESET_DEL: 
        break;

      case HUB_MENU:

        INTERFACE_CURSOR_POSITION--;
        if(INTERFACE_CURSOR_POSITION < 0) INTERFACE_CURSOR_POSITION = 3;
        break;

      case PROCESSING: 
        break;

      case FX_EDIT:

        INTERFACE_CURSOR_POSITION--;
        if(INTERFACE_CURSOR_POSITION < 0) INTERFACE_CURSOR_POSITION = 4;
        break;

      case FX_ID_CHANGE:

        INTERFACE_CURSOR_POSITION--;

        if(PAGINA_VIRTUAL <= 0 && INTERFACE_CURSOR_POSITION == -1)
        {
          PAGINA_VIRTUAL = AMOUNT_FX/5;
          INTERFACE_CURSOR_POSITION = AMOUNT_FX % 5;
        }
        else if (INTERFACE_CURSOR_POSITION == -1)
        {
          PAGINA_VIRTUAL--;
          INTERFACE_CURSOR_POSITION = 4;
        }
        break;

      case FX_PARAM_EDIT:

        // passo grande
        if(STEP_SIZE == 0)
        {
          switch(PRESET[ACTIVE_EFFECT] -> recuperarID())
          {
            case MACRO_BUFFER:
              break;

            case MACRO_SOFT_AMP:

              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX -= 0.5;
                  if(EDIT_AUX < 0) EDIT_AUX = 10;
                  break;

                default:
                  break;
              }
              break;

            case MACRO_HARD_AMP:

              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX -= 0.5;
                  if(EDIT_AUX < 0) EDIT_AUX = 10;
                  break;

                default:
                  break;
              }
              break;

            case MACRO_SOFT_SAT:

              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX -= 0.5;
                  if(EDIT_AUX < 0) EDIT_AUX = 10;
                  break;

                case 1:
                  EDIT_AUX -= 0.05;
                  if(EDIT_AUX < 0) EDIT_AUX = 1;
                  break;

                default:
                  break;
              }
              break;

            case MACRO_HARD_SAT:
              
              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX -= 0.5;
                  if(EDIT_AUX < 0) EDIT_AUX = 10;
                  break;

                case 1:
                  EDIT_AUX -= 0.05;
                  if(EDIT_AUX < 0) EDIT_AUX = 1;
                  break;

                default:
                  break;
              }
              break;

            case MACRO_ECHO:

              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX -= 0.05;
                  if(EDIT_AUX < 0) EDIT_AUX = 1;
                  break;

                case 1:
                  EDIT_AUX -= 0.05;
                  if(EDIT_AUX < 0) EDIT_AUX = 2;
                  break;
                
                case 2:
                  EDIT_AUX -= 0.05;
                  if(EDIT_AUX < 0) EDIT_AUX = 1;
                  break;

                default:
                  break;
              }
              break;

            case MACRO_LOWPASS:

              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX -= 0.05;
                  if(EDIT_AUX < 0) EDIT_AUX = 1;
                  break;

                case 1:
                  EDIT_AUX -= 500;
                  if(EDIT_AUX < 0) EDIT_AUX = 20000;
                  break;

                default:
                  break;
              }
              break;

            case MACRO_HIGHPASS:

              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX -= 0.05;
                  if(EDIT_AUX < 0) EDIT_AUX = 1;
                  break;

                case 1:
                  EDIT_AUX -= 500;
                  if(EDIT_AUX < 0) EDIT_AUX = 20000;
                  break;

                default:
                  break;
              }
              break;

            case MACRO_NOTCH:

              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX -= 0.05;
                  if(EDIT_AUX < 0) EDIT_AUX = 1;
                  break;

                case 1:
                  EDIT_AUX -= 500;
                  if(EDIT_AUX < 0) EDIT_AUX = 20000;
                  break;

                default:
                  break;
              }
              break;
          }
        }
        // passo pequeno
        else
        {
          switch(PRESET[ACTIVE_EFFECT] -> recuperarID())
          {
            case MACRO_BUFFER:
              break;

            case MACRO_SOFT_AMP:

              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX -= 0.1;
                  if(EDIT_AUX < 0) EDIT_AUX = 10;
                  break;

                default:
                  break;
              }
              break;

            case MACRO_HARD_AMP:

              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX -= 0.1;
                  if(EDIT_AUX < 0) EDIT_AUX = 10;
                  break;

                default:
                  break;
              }
              break;

            case MACRO_SOFT_SAT:

              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX -= 0.1;
                  if(EDIT_AUX < 0) EDIT_AUX = 10;
                  break;

                case 1:
                  EDIT_AUX -= 0.01;
                  if(EDIT_AUX < 0) EDIT_AUX = 1;
                  break;

                default:
                  break;
              }
              break;

            case MACRO_HARD_SAT:

              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX -= 0.5;
                  if(EDIT_AUX < 0) EDIT_AUX = 10;
                  break;

                case 1:
                  EDIT_AUX -= 0.05;
                  if(EDIT_AUX < 0) EDIT_AUX = 1;
                  break;

                default:
                  break;
              }
              break;

            case MACRO_ECHO:

              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX -= 0.01;
                  if(EDIT_AUX < 0) EDIT_AUX = 1;
                  break;

                case 1:
                  EDIT_AUX -= 0.01;
                  if(EDIT_AUX < 0) EDIT_AUX = 2;
                  break;
                
                case 2:
                  EDIT_AUX -= 0.01;
                  if(EDIT_AUX < 0) EDIT_AUX = 1;
                  break;

                default:
                  break;
              }
              break;

            case MACRO_LOWPASS:

              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX -= 0.01;
                  if(EDIT_AUX < 0) EDIT_AUX = 1;
                  break;

                case 1:
                  EDIT_AUX -= 10;
                  if(EDIT_AUX < 0) EDIT_AUX = 20000;
                  break;

                default:
                  break;
              }
              break;

            case MACRO_HIGHPASS:

              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX -= 0.01;
                  if(EDIT_AUX < 0) EDIT_AUX = 1;
                  break;

                case 1:
                  EDIT_AUX -= 10;
                  if(EDIT_AUX < 0) EDIT_AUX = 20000;
                  break;

                default:
                  break;
              }
              break;

            case MACRO_NOTCH:

              switch(ACTIVE_PARAM)
              {
                case 0:
                  EDIT_AUX -= 0.01;
                  if(EDIT_AUX < 0) EDIT_AUX = 1;
                  break;

                case 1:
                  EDIT_AUX -= 10;
                  if(EDIT_AUX < 0) EDIT_AUX = 20000;
                  break;

                default:
                  break;
              }
              break;
          }
        }
        break;
    }
  }
  else if(getDifferenceEncoderSwitch() && NEW_STATUS_ENCODER_SW == 1)
  {
    switch(INTERFACE_STATE)
    {
      case PRESET_SELECT:

        break;

      case PRESET_EDIT:

        INTERFACE_STATE = PROCESSING;
        INTERFACE_CURSOR_POSITION = 0;
        PAGINA_VIRTUAL = 0;
        break;

      case PRESET_DEL: 

        // deleta o preset ativo atualmente
        AMOUNT_PRESETS--;

        //reset total
        zerarPreset();
        INTERFACE_STATE = PRESET_SELECT;
        INTERFACE_CURSOR_POSITION = 0;
        PAGINA_VIRTUAL = 0;
        ACTIVE_PRESET_FILE = 0;
        break;

      case HUB_MENU:

        INTERFACE_STATE = PROCESSING;
        INTERFACE_CURSOR_POSITION = 0;
        PAGINA_VIRTUAL = 0;
        break;

      case PROCESSING: 

        break;

      case FX_EDIT: 

        break;

      case FX_ID_CHANGE: 

        break;

      case FX_PARAM_EDIT: 

        // troca o tamanho do step
        STEP_SIZE++;
        STEP_SIZE = STEP_SIZE % 2;
        break;
    }
  }
}

int Status::getDifferenceButA()
{
  return ((OLD_STATUS_BUT_A != NEW_STATUS_BUT_A));
}

int Status::getDifferenceButB()
{
  return ((OLD_STATUS_BUT_B != NEW_STATUS_BUT_B));
}

int Status::getDifferenceEncoderSwitch()
{
  return ((OLD_STATUS_ENCODER_SW != NEW_STATUS_ENCODER_SW));
}

int Status::incrementarEncoderPulseCounter()
{
  //if(ENCODER_PULSE_COUNTER < 0) ENCODER_PULSE_COUNTER = 0;
  ENCODER_PULSE_COUNTER++;
}

int Status::decrementarEncoderPulseCounter()
{
  //if(ENCODER_PULSE_COUNTER > 0) ENCODER_PULSE_COUNTER = 0;
  ENCODER_PULSE_COUNTER--;
}

int Status::getCursorUp()
{
  if(ENCODER_PULSE_COUNTER > ENCODER_COUNTER_THRESHOLD) 
  { 
    return 1;
  }
  else return 0;
}

int Status::getCursorDown()
{
  if(ENCODER_PULSE_COUNTER < -ENCODER_COUNTER_THRESHOLD)
  {
    return 1;
  }
  else return 0;
}
