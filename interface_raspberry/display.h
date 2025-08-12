#ifndef DISPLAY_H
#define DISPLAY_H

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// vermelho: 200, 15, 20
// magenta: 255, 100, 255
// verde: 65, 255, 140
// azul: 0, 50, 200
// ciano: 0, 210, 225
// amarelo: 255, 200, 15
// branco: 255, 255, 255
// preto: 0, 0, 0

int cairo_check_event(cairo_surface_t *sfc, int block);
static void fullscreen(Display* dpy, Window win);
cairo_surface_t *cairo_create_x11_surface(int *x, int *y);
void cairo_close_x11_surface(cairo_surface_t *sfc);

void display_img(cairo_t *ctx, cairo_surface_t *img, int pos_x, int pos_y);
void display_hub(cairo_t *ctx, cairo_surface_t **img_hub, cairo_surface_t **img_sel, int sel);
void display_sel_button(cairo_t *ctx, const char *label, int pos, bool sel);
void display_sel_screen(cairo_t *ctx, int amountPresets, int pagina, int cursor);
void display_preset_edit_screen(cairo_t *ctx, int cursor);
void display_id_screen(cairo_t *ctx, int amountFxTypes, int pagina, int cursor);
void display_fx_screen(cairo_t *ctx, char fxLabel, float parametros[], int cursor);

#endif
