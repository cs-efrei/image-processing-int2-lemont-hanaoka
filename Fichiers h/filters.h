#ifndef FILTERS_H
#define FILTERS_H

float **createBoxBlurKernel();
float **createGaussianBlurKernel();
float **createOutlineKernel();
float **createEmbossKernel();
float **createSharpenKernel();

float **allocateKernel(int size);
void freeKernel(float **kernel, int size);

#endif