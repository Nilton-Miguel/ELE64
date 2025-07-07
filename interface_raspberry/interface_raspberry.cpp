#include <iostream>
#include "status.h"
#include <wiringPi.h>
#include <unistd.h>

Status status(BUT_A, BUT_B, ENCODER_DATA, ENCODER_CLK, ENCODER_SW);

void HANDLER_CLK()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();

  if (interrupt_time - last_interrupt_time > ENCODER_DEBOUNCE) 
  {
    if(digitalRead(ENCODER_DATA) != digitalRead(ENCODER_CLK)) 
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

void HANDLER_DATA()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();

  if (interrupt_time - last_interrupt_time > ENCODER_DEBOUNCE) 
  {
    if (digitalRead(ENCODER_CLK) == digitalRead(ENCODER_DATA)) 
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

int main() {

  wiringPiSetupPinType(WPI_PIN_WPI);

  pinMode(BUT_A,          INPUT);
  pinMode(BUT_B,          INPUT);
  pinMode(ENCODER_SW,     INPUT);

  pinMode(ENCODER_DATA,   INPUT);
  pinMode(ENCODER_CLK,    INPUT);

  pullUpDnControl(BUT_A, PUD_UP);
  pullUpDnControl(BUT_B, PUD_UP);

  //attachInterrupt(digitalPinToInterrupt(ENCODER_CLK),   HANDLER_CLK, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(ENCODER_DATA),  HANDLER_DATA, CHANGE);
  wiringPiISR(ENCODER_CLK, INT_EDGE_BOTH, HANDLER_CLK);
  wiringPiISR(ENCODER_DATA, INT_EDGE_BOTH, HANDLER_DATA);

  //Serial.begin(31250);

  while(1) {

    if(status.getActivity())
    {
      // esses dados agora podem ser acessados publicamente
      std::cout << "PRESET: ";
      std::cout << status.getActivePreset();

      std::cout << " ESTADO: ";
      std::cout << status.getInterfaceState();

      std::cout << " OPÇÃO: ";
      std::cout << 5*status.getPaginaVirtual() + status.getCursorPosition();

      if(status.getInterfaceState() > 4) {
        std::cout << " EFEITO: ";
        std::cout << status.getActiveEffect();

        std::cout << " ID: ";
        std::cout << (int) status.getPresetBuffer()[status.getActiveEffect()] -> recuperarID();

        std::cout << " PARAMETROS: ";
        std::cout << status.getPresetBuffer()[status.getActiveEffect()] -> recuperarParametro(0) << " ";
        std::cout << status.getPresetBuffer()[status.getActiveEffect()] -> recuperarParametro(1) << " ";
        std::cout << status.getPresetBuffer()[status.getActiveEffect()] -> recuperarParametro(2) << " ";
        std::cout << status.getPresetBuffer()[status.getActiveEffect()] -> recuperarParametro(3) << " ";

        std::cout << " PARAM ATIVO: ";
        std::cout << status.getActiveParam();

        std::cout << " AUX: ";
        std::cout << status.getEditAux();
      }
      std::cout << "\n";
    }

    status.updateStatus();
    usleep(10000);
  }
}
