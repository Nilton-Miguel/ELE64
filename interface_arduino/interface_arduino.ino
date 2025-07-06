
#include "status.h"

Status status(BUT_A, BUT_B, ENCODER_DATA, ENCODER_CLK, ENCODER_SW);

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

  status.updateStatus();

  if(status.getActivity())
  {
    // esses dados agora podem ser acessados publicamente
    Serial.print("ESTADO: ");
    Serial.print(status.getInterfaceState());

    Serial.print(" CURSOR: ");
    Serial.print(status.getCursorPosition());

    Serial.print(" PAGINA: ");
    Serial.print(status.getPaginaVirtual());

    Serial.print(" PRESET: ");
    Serial.print(status.getActivePreset());

    Serial.print(" EFEITO: ");
    Serial.print(status.getActiveEffect());

    Serial.print(" PARAM: ");
    Serial.print(status.getActiveParam());

    Serial.print(" AUX: ");
    Serial.print(status.getEditAux());

    Serial.print(" ID FX: ");
    Serial.print((int) status.getPresetBuffer()[0] -> recuperarID());

    Serial.print(" PR0 FX: ");
    Serial.print(status.getPresetBuffer()[0] -> recuperarParametro(0));

    Serial.println();
  }

  delay(10);
}
