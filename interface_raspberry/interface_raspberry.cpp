#include <iostream>
#include <wiringPi.h>
#include <unistd.h>

#include "status.h"
#include "display.h"

StatusClass myStatus(BUT_A, BUT_B, ENCODER_DATA, ENCODER_CLK, ENCODER_SW);

void HANDLER_CLK()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();

  if (interrupt_time - last_interrupt_time > ENCODER_DEBOUNCE) 
  {
    if(digitalRead(ENCODER_DATA) != digitalRead(ENCODER_CLK)) 
    {
      myStatus.incrementarEncoderPulseCounter();
    } 
    else 
    {
      myStatus.decrementarEncoderPulseCounter();
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
      myStatus.incrementarEncoderPulseCounter();
    } 
    else 
    {
      myStatus.decrementarEncoderPulseCounter();
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

  cairo_surface_t *sfc;
  cairo_t *ctx;
  int x = 0;
  int y = 0;

  sfc = cairo_create_x11_surface(&x, &y);
  ctx = cairo_create(sfc);


  cairo_surface_t *img;
  img = cairo_image_surface_create_from_png("/home/raspberrypi/DEL_ONLY.png");
  display_img(ctx, sfc, img);

  bool running = true;
  while(running) {

    if(myStatus.getActivity())
    {
      // esses dados agora podem ser acessados publicamente
      std::cout << "PRESET: ";
      std::cout << myStatus.getActivePreset();

      std::cout << " ESTADO: ";
      std::cout << myStatus.getInterfaceState();

      std::cout << " OPÇÃO: ";
      std::cout << 5*myStatus.getPaginaVirtual() + myStatus.getCursorPosition();

      if(myStatus.getInterfaceState() > 4) {
        std::cout << " EFEITO: ";
        std::cout << myStatus.getActiveEffect();

        std::cout << " ID: ";
        std::cout << (int) myStatus.getPresetBuffer()[myStatus.getActiveEffect()] -> recuperarID();

        std::cout << " PARAMETROS: ";
        std::cout << myStatus.getPresetBuffer()[myStatus.getActiveEffect()] -> recuperarParametro(0) << " ";
        std::cout << myStatus.getPresetBuffer()[myStatus.getActiveEffect()] -> recuperarParametro(1) << " ";
        std::cout << myStatus.getPresetBuffer()[myStatus.getActiveEffect()] -> recuperarParametro(2) << " ";
        std::cout << myStatus.getPresetBuffer()[myStatus.getActiveEffect()] -> recuperarParametro(3) << " ";

        std::cout << " PARAM ATIVO: ";
        std::cout << myStatus.getActiveParam();

        std::cout << " AUX: ";
        std::cout << myStatus.getEditAux();
      }
      std::cout << "\n";
    }

    switch(cairo_check_event(sfc, 0)) {
      case 0xff1b:  // Esc no teclado
        running = false;
        break;
      default:
        break;
    }

    myStatus.updateStatus();
    usleep(10000);
  }

  cairo_destroy(ctx);
  cairo_close_x11_surface(sfc);

  return 0;
}
