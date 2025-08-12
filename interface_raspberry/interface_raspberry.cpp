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


  cairo_select_font_face(ctx, "Georgia", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(ctx, 24.0);

  cairo_surface_t *img_del;
  img_del = cairo_image_surface_create_from_png("/home/raspberrypi/DEL_ONLY.png");
  cairo_surface_t *img_proc;
  img_proc = cairo_image_surface_create_from_png("/home/raspberrypi/PROC_ONLY.png");
	cairo_surface_t *img_hub[4];
	img_hub[0] = cairo_image_surface_create_from_png("/home/raspberrypi/USAR_NON.png");
	img_hub[1] = cairo_image_surface_create_from_png("/home/raspberrypi/TROCAR_NON.png");
	img_hub[2] = cairo_image_surface_create_from_png("/home/raspberrypi/UDITSAR_NON.png");
	img_hub[3] = cairo_image_surface_create_from_png("/home/raspberrypi/DEL_NON.png");
	cairo_surface_t *img_sel[4];
	img_sel[0] = cairo_image_surface_create_from_png("/home/raspberrypi/USAR_SEL.png");
	img_sel[1] = cairo_image_surface_create_from_png("/home/raspberrypi/TROCAR_SEL.png");
	img_sel[2] = cairo_image_surface_create_from_png("/home/raspberrypi/UDITSAR_SEL.png");
	img_sel[3] = cairo_image_surface_create_from_png("/home/raspberrypi/DEL_SEL.png");



  bool running = true;
  while(running) {

    /*
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
    */
  if(myStatus.getActivity()) {
    switch(myStatus.getInterfaceState()) {
      default:
      case PRESET_SELECT:
        cairo_push_group(ctx);
        display_sel_screen(ctx, myStatus.getAmountPresets(), myStatus.getPaginaVirtual(), myStatus.getCursorPosition());
        cairo_pop_group_to_source(ctx);
        cairo_paint(ctx);
        cairo_surface_flush(sfc);
        break;
      case PRESET_EDIT:
        cairo_push_group(ctx);
        display_preset_edit_screen(ctx, myStatus.getCursorPosition());
        cairo_pop_group_to_source(ctx);
        cairo_paint(ctx);
        cairo_surface_flush(sfc);
        break;
      case PRESET_DEL:
        cairo_push_group(ctx);
        display_img(ctx, img_del, 0, 0);
	      cairo_pop_group_to_source(ctx);
	      cairo_paint(ctx);
        cairo_surface_flush(sfc);
        break;
      case HUB_MENU:
	      cairo_push_group(ctx);
        display_hub(ctx, img_hub, img_sel, myStatus.getOption());
	      cairo_pop_group_to_source(ctx);
	      cairo_paint(ctx);
        cairo_surface_flush(sfc);
        break;
      case PROCESSING:
        cairo_push_group(ctx);
        display_img(ctx, img_proc, 0, 0);
	      cairo_pop_group_to_source(ctx);
	      cairo_paint(ctx);
        cairo_surface_flush(sfc);
        cairo_check_event(sfc, 0);
        myStatus.startProcessing();
        break;
      case FX_EDIT: {
        cairo_push_group(ctx);
        Efeito *meuEfeito = myStatus.getPresetBuffer()[myStatus.getActiveEffect()];
        char fxLabel = meuEfeito->recuperarID();
        float fxParam[4];
        for(int i = 0; i < 4; i++) {
          fxParam[i] = meuEfeito->recuperarParametro(i);
        }
        display_fx_screen(ctx, fxLabel, fxParam, myStatus.getCursorPosition());
        cairo_pop_group_to_source(ctx);
        cairo_paint(ctx);
        cairo_surface_flush(sfc);
        break;
      }
      case FX_ID_CHANGE: {
        cairo_push_group(ctx);
        display_id_screen(ctx, myStatus.getAmountFx(), myStatus.getPaginaVirtual(), myStatus.getCursorPosition());
        cairo_pop_group_to_source(ctx);
        cairo_paint(ctx);
        cairo_surface_flush(sfc);
        break;
    }
      case FX_PARAM_EDIT: {
        cairo_push_group(ctx);
        Efeito *meuEfeito = myStatus.getPresetBuffer()[myStatus.getActiveEffect()];
        char fxLabel = meuEfeito->recuperarID();
        float fxParam[4];
        for(int i = 0; i < 4; i++) {
          if(i == (myStatus.getCursorPosition() - 1)) {
            fxParam[i] = myStatus.getEditAux();
            continue;
          }
          fxParam[i] = meuEfeito->recuperarParametro(i);
        }
        display_fx_screen(ctx, fxLabel, fxParam, myStatus.getCursorPosition());
        cairo_pop_group_to_source(ctx);
        cairo_paint(ctx);
        cairo_surface_flush(sfc);
        break;
      }
    }
  }

    switch(cairo_check_event(sfc, 0)) {
      case 0xff1b:  // Esc no teclado
        running = false;
        break;
      default:
        break;
    }

    myStatus.updateStatus();
    usleep(100000);
  }

  cairo_destroy(ctx);
  cairo_close_x11_surface(sfc);

  return 0;
}
