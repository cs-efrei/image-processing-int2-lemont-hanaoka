#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Fichiers h/bmp8.h"

t_bmp8 *bmp8_loadImage(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) return NULL;
    t_bmp8 *img = malloc(sizeof(t_bmp8));
    if (!img) { fclose(f); return NULL; }
    fread(img->header, 1, 54, f);
    fread(img->colorTable, 1, 1024, f);
    img->width = *(unsigned int *)&img->header[18];
    img->height = *(unsigned int *)&img->header[22];
    img->colorDepth = *(unsigned short *)&img->header[28];
    img->dataSize = *(unsigned int *)&img->header[34];
    img->data = malloc(img->width * img->height);
    if (!img->data) { free(img); fclose(f); return NULL; }
    fread(img->data, 1, img->width * img->height, f);
    fclose(f);
    return img;
}

void bmp8_saveImage(const char *filename, t_bmp8 *img) {
    FILE *f = fopen(filename, "wb");
    if (!f) return;
    fwrite(img->header, 1, 54, f);
    fwrite(img->colorTable, 1, 1024, f);
    fwrite(img->data, 1, img->width * img->height, f);
    fclose(f);
}

void bmp8_free(t_bmp8 *img) {
    if (img) {
        free(img->data);
        free(img);
    }
}

void bmp8_printInfo(t_bmp8 *img) {
    printf("Image Info:\n");
    printf("Width: %u\n", img->width);
    printf("Height: %u\n", img->height);
    printf("Color Depth: %u\n", img->colorDepth);
    printf("Data Size: %u\n", img->dataSize);
}

void bmp8_negative(t_bmp8 *img) {
    for (unsigned int i = 0; i < img->width * img->height; i++)
        img->data[i] = 255 - img->data[i];
}

void bmp8_brightness(t_bmp8 *img, int value) {
    for (unsigned int i = 0; i < img->width * img->height; i++) {
        int p = img->data[i] + value;
        if (p > 255) p = 255;
        if (p < 0) p = 0;
        img->data[i] = (unsigned char)p;
    }
}

void bmp8_threshold(t_bmp8 *img, int threshold) {
    for (unsigned int i = 0; i < img->width * img->height; i++)
        img->data[i] = (img->data[i] >= threshold) ? 255 : 0;
}