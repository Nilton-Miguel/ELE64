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


int cairo_check_event(cairo_surface_t *sfc, int block);
static void fullscreen(Display* dpy, Window win);
cairo_surface_t *cairo_create_x11_surface(int *x, int *y);
void cairo_close_x11_surface(cairo_surface_t *sfc);

void display_img(cairo_t *ctx, cairo_surface_t *img, int pos_x, int pos_y);
void display_hub(cairo_t *ctx, cairo_surface_t **img_hub, cairo_surface_t **img_sel, int sel);

#endif
