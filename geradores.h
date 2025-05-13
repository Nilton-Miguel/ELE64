
#ifndef GERADORES_H
#define GERADORES_H

#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <fftw3.h>

fftw_complex *media_movel(int SAMPLING_WINDOW_SIZE, int ORDER);

#endif
