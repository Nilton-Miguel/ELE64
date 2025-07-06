
#include "status.h"

Status status(BUT_A, BUT_B, ENCODER_DATA, ENCODER_CLK, ENCODER_SW);

int display_command = 0;
int display_choice = 0;

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

void setup() {

  pinMode(BUT_A,          INPUT);
  pinMode(BUT_B,          INPUT);
  pinMode(ENCODER_SW,     INPUT);

  pinMode(ENCODER_DATA,   INPUT);
  pinMode(ENCODER_CLK,    INPUT);

  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK),   HANDLER_CLK, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_DATA),  HANDLER_DATA, CHANGE);

  Serial.begin(31250);
}

void loop() {

  display_command = Serial.read();
  switch(display_command)
  {
    case 32:
      // space bar para ver a maquina de estados
      display_choice = 0;
      break;

    case 49:
      // 1 para ver o efeito 1
      display_choice = 1;
      break;

    case 50:
      // 2 para ver o efeito 2
      display_choice = 2;
      break;

    case 51:
      // 3 para ver o efeito 3
      display_choice = 3;
      break;
  }

  status.updateStatus();

  if(status.getActivity())
  {

    // esses dados agora podem ser acessados publicamente
    Serial.print(status.getInterfaceState());
    Serial.print(" ");
    Serial.print(status.getCursorPosition());
    Serial.print(" ");
    Serial.print(status.getPaginaVirtual());
    Serial.print(" ");
    Serial.print(status.getActivePreset());
    Serial.print(" ");
    Serial.print(status.getActiveEffect());
    Serial.print(" ");
    Serial.print(status.getActiveParam());
    Serial.print(" ");

    Serial.println();
  }

  /*
  if(status.getActivity())
  {

    switch(display_choice)
    {
      case 1:
        
        status.printEfeito(0);
        break;
      
      case 2:
        status.printEfeito(1);
        break;
      
      case 3:
        status.printEfeito(2);
        break;

      default:
        status.printStatus();
    }
  }
  */

  delay(10);
}
