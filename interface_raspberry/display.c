#include "display.h"

/*! Check for Xlib Mouse/Keypress events. All other events are discarded. 
 * @param sfc Pointer to Xlib surface.
 * @param block If block is set to 0, this function always returns immediately
 * and does not block. if set to a non-zero value, the function will block
 * until the next event is received.
 * @return The function returns 0 if no event occured (and block is set). A
 * positive value indicates that a key was pressed and the X11 key symbol as
 * defined in <X11/keysymdef.h> is returned. A negative value indicates a mouse
 * button event. -1 is button 1 (left button), -2 is the middle button, and -3
 * the right button.
 */
int cairo_check_event(cairo_surface_t *sfc, int block)
{
   char keybuf[8];
   KeySym key;
   XEvent e;

   for (;;)
   {
      if (block || XPending(cairo_xlib_surface_get_display(sfc)))
         XNextEvent(cairo_xlib_surface_get_display(sfc), &e);
      else 
         return 0;

      switch (e.type)
      {
         case ButtonPress:
            return -e.xbutton.button;
         case KeyPress:
            XLookupString(&e.xkey, keybuf, sizeof(keybuf), &key, NULL);
            return key;
         default:
            fprintf(stderr, "Dropping unhandled XEevent.type = %d.\n", e.type);
      }
   }
}


static void fullscreen(Display* dpy, Window win)
{
  Atom atoms[2] = { XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False), None };
  XChangeProperty(dpy, win, XInternAtom(dpy, "_NET_WM_STATE", False),
                  XA_ATOM, 32, PropModeReplace, (unsigned char*) atoms, 1);
}


/*! Open an X11 window and create a cairo surface base on that window. If x and
 * y are set to 0 the function opens a full screen window and stores to window
 * dimensions to x and y.
 * @param x Pointer to width of window.
 * @param y Pointer to height of window.
 * @return Returns a pointer to a valid Xlib cairo surface. The function does
 * not return on error (exit(3)).
 */
cairo_surface_t *cairo_create_x11_surface(int *x, int *y)
{
   Display *dsp;
   Drawable da;
   Screen *scr;
   int screen;
   cairo_surface_t *sfc;

   if ((dsp = XOpenDisplay(NULL)) == NULL)
      exit(1);
   screen = DefaultScreen(dsp);
   scr = DefaultScreenOfDisplay(dsp);
   if (!*x || !*y)
   {
      *x = WidthOfScreen(scr), *y = HeightOfScreen(scr);
      da = XCreateSimpleWindow(dsp, DefaultRootWindow(dsp), 0, 0, *x, *y, 0, 0, 0);
      fullscreen (dsp, da);
   }
   else
      da = XCreateSimpleWindow(dsp, DefaultRootWindow(dsp), 0, 0, *x, *y, 0, 0, 0);
   XSelectInput(dsp, da, ButtonPressMask | KeyPressMask);
   XMapWindow(dsp, da);

   sfc = cairo_xlib_surface_create(dsp, da, DefaultVisual(dsp, screen), *x, *y);
   cairo_xlib_surface_set_size(sfc, *x, *y);

   return sfc;
}


/*! Destroy cairo Xlib surface and close X connection.
 */
void cairo_close_x11_surface(cairo_surface_t *sfc)
{
   Display *dsp = cairo_xlib_surface_get_display(sfc);

   cairo_surface_destroy(sfc);
   XCloseDisplay(dsp);
}

void display_img(cairo_t *ctx, cairo_surface_t *img, int pos_x, int pos_y) {
   int width = cairo_image_surface_get_width(img);
   int height = cairo_image_surface_get_height(img);
	cairo_rectangle(ctx, pos_x, pos_y, width, height);
	cairo_set_source_surface(ctx, img, pos_x, pos_y);
   cairo_fill(ctx);
}

void display_hub(cairo_t *ctx, cairo_surface_t **img_hub, cairo_surface_t **img_sel, int sel) {
   display_img(ctx, img_hub[0], 0, 0);
   display_img(ctx, img_hub[1], 159, 0);
   display_img(ctx, img_hub[2], 0, 119);
   display_img(ctx, img_hub[3], 159, 119);
   int sel_pos_x = (sel % 2) * 159;
   int sel_pos_y = (sel / 2) * 119;
   display_img(ctx, img_sel[sel], sel_pos_x, sel_pos_y);
}

void display_sel_button(cairo_t *ctx, const char *label, int pos, bool sel) {
   int pos_box_y = pos * 48;
   int pos_text_y = 32 + pos * 48;
   cairo_set_source_rgb(ctx, 1.0, 1.0, 1.0);
   if(sel) {
      cairo_rectangle(ctx, 0, pos_box_y, 320, 48);
      cairo_fill(ctx);
      cairo_set_source_rgb(ctx, 0.0, 0.0, 0.0);
   }
   cairo_move_to(ctx, 16, pos_text_y);
   cairo_show_text(ctx, label);
}
void display_sel_screen(cairo_t *ctx, int amountPresets, int pagina, int cursor) {
   cairo_set_source_rgb(ctx, 0.0, 0.0, 0.0);
   cairo_paint(ctx);

   char label[] = "A";
   if(pagina > 0) {
      // Por causa do item "+" incrementamos 4 após a primeira página. Após as subsequentes incrementamos 5
      label[0] += 4 + 5*(pagina - 1);
   }

   for(int i = 0; i < 5; i++) {
      bool isSel = (i == cursor)? true : false;
      // O primeiro item da primeira página é o de criar novo preset
      if(pagina == 0 && i == 0) {
         display_sel_button(ctx, "+", 0, isSel);
         // não precisamos incrementar label[0] antes do próximo loop, pois o primeiro preset ("A") ainda não foi mostrado
         continue;
      }
      // Só podemos mostrar presets que existem
      if((5*pagina + i) > amountPresets) {
         break;
      }
      display_sel_button(ctx, label, i % 5, isSel);
      ++label[0];
   }
}
void display_preset_edit_screen(cairo_t *ctx, int cursor) {
   cairo_set_source_rgb(ctx, 0.0, 0.0, 0.0);
   cairo_paint(ctx);

   for(int i = 0; i < 3; i++) {
      bool isSel = (i == cursor)? true : false;
      char dispString[10];
      sprintf(dispString, "Efeito %d", i + 1);
      display_sel_button(ctx, dispString, i, isSel);
   }
}
void display_fx_screen(cairo_t *ctx, char fxLabel, float fxParam[], int cursor) {
   cairo_set_source_rgb(ctx, 0.0, 0.0, 0.0);
   cairo_paint(ctx);
   for(int i = 0; i < 5; i++) {
      bool isSel = (i == cursor)? true : false;
      if(i == 0) {
         char dispString[10];
      switch(fxLabel) {
         case 0:
            sprintf(dispString, "BUFFER");
            break;
         case 1:
            sprintf(dispString, "SOFT AMP");
            break;
         case 2:
            sprintf(dispString, "HARD AMP");
            break;
         case 3:
            sprintf(dispString, "SOFT SAT");
            break;
         case 4:
            sprintf(dispString, "HARD SAT");
            break;
         case 5:
            sprintf(dispString, "ECHO");
            break;
         case 6:
            sprintf(dispString, "LOW PASS");
            break;
         case 7:
            sprintf(dispString, "HIGH PASS");
            break;
         case 8:
            sprintf(dispString, "NOTCH");
            break;
         default:
            sprintf(dispString, "?");
            break;
      }
         display_sel_button(ctx, dispString, 0, isSel);
         continue;
      }
      char paramVal[10];
      sprintf(paramVal, "%.2f", fxParam[i-1]);
      display_sel_button(ctx, paramVal, i, isSel);
   }
}
void display_id_screen(cairo_t *ctx, int amountFxTypes, int pagina, int cursor) {
   cairo_set_source_rgb(ctx, 0.0, 0.0, 0.0);
   cairo_paint(ctx);

   for(int i = 0; i < 5; i++) {
      bool isSel = (i == cursor)? true : false;
      int indice = 5*pagina + i;

      // Só podemos mostrar efeitos que existem
      if(indice >= amountFxTypes) {
         break;
      }
      char dispString[10];
      switch(indice) {
         case 0:
            sprintf(dispString, "BUFFER");
            break;
         case 1:
            sprintf(dispString, "SOFT AMP");
            break;
         case 2:
            sprintf(dispString, "HARD AMP");
            break;
         case 3:
            sprintf(dispString, "SOFT SAT");
            break;
         case 4:
            sprintf(dispString, "HARD SAT");
            break;
         case 5:
            sprintf(dispString, "ECHO");
            break;
         case 6:
            sprintf(dispString, "LOW PASS");
            break;
         case 7:
            sprintf(dispString, "HIGH PASS");
            break;
         case 8:
            sprintf(dispString, "NOTCH");
            break;
         default:
            sprintf(dispString, "?");
            break;
      }
      display_sel_button(ctx, dispString, i, isSel);
   }
}
