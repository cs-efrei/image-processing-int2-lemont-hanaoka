#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "bmp8.h"

unsigned int *bmp8_computeHistogram(t_bmp8 *img);
unsigned int *bmp8_computeCDF(unsigned int *hist, unsigned int totalPixels);
void bmp8_equalize(t_bmp8 *img);

#endif
