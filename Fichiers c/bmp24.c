#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "Fichiers h/bmp24.h"

// --- Allocation/Deallocation ---
t_pixel **bmp24_allocateDataPixels(int width, int height) {
    t_pixel **pixels = malloc(height * sizeof(t_pixel *));
    if (!pixels) return NULL;
    for (int i = 0; i < height; i++) {
        pixels[i] = malloc(width * sizeof(t_pixel));
        if (!pixels[i]) {
            for (int j = 0; j < i; j++) free(pixels[j]);
            free(pixels);
            return NULL;
        }
    }
    return pixels;
}

void bmp24_freeDataPixels(t_pixel **pixels, int height) {
    if (!pixels) return;
    for (int i = 0; i < height; i++) free(pixels[i]);
    free(pixels);
}

t_bmp24 *bmp24_allocate(int width, int height, int colorDepth) {
    t_bmp24 *img = malloc(sizeof(t_bmp24));
    if (!img) return NULL;
    img->width = width;
    img->height = height;
    img->colorDepth = colorDepth;
    img->data = bmp24_allocateDataPixels(width, height);
    if (!img->data) { free(img); return NULL; }
    return img;
}

void bmp24_free(t_bmp24 *img) {
    if (img) {
        bmp24_freeDataPixels(img->data, img->height);
        free(img);
    }
}

// --- I/O ---
t_bmp24 *bmp24_loadImage(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) return NULL;
    t_bmp24 *img = malloc(sizeof(t_bmp24));
    if (!img) { fclose(f); return NULL; }
    fread(&img->header, sizeof(t_bmp_header), 1, f);
    fread(&img->header_info, sizeof(t_bmp_info), 1, f);
    img->width = img->header_info.width;
    img->height = img->header_info.height;
    img->colorDepth = img->header_info.bits;
    img->data = bmp24_allocateDataPixels(img->width, img->height);
    int padding = (4 - (img->width * 3) % 4) % 4;
    for (int i = img->height - 1; i >= 0; i--) {
        fread(img->data[i], sizeof(t_pixel), img->width, f);
        fseek(f, padding, SEEK_CUR);
    }
    fclose(f);
    return img;
}

void bmp24_saveImage(const char *filename, t_bmp24 *img) {
    FILE *f = fopen(filename, "wb");
    if (!f) return;
    fwrite(&img->header, sizeof(t_bmp_header), 1, f);
    fwrite(&img->header_info, sizeof(t_bmp_info), 1, f);
    int padding = (4 - (img->width * 3) % 4) % 4;
    unsigned char pad[3] = {0, 0, 0};
    for (int i = img->height - 1; i >= 0; i--) {
        fwrite(img->data[i], sizeof(t_pixel), img->width, f);
        fwrite(pad, 1, padding, f);
    }
    fclose(f);
}

// --- Processing ---
void bmp24_negative(t_bmp24 *img) {
    for (int i = 0; i < img->height; i++)
        for (int j = 0; j < img->width; j++) {
            img->data[i][j].red   = 255 - img->data[i][j].red;
            img->data[i][j].green = 255 - img->data[i][j].green;
            img->data[i][j].blue  = 255 - img->data[i][j].blue;
        }
}

void bmp24_grayscale(t_bmp24 *img) {
    for (int i = 0; i < img->height; i++)
        for (int j = 0; j < img->width; j++) {
            uint8_t gray = (uint8_t)(0.299 * img->data[i][j].red +
                                     0.587 * img->data[i][j].green +
                                     0.114 * img->data[i][j].blue);
            img->data[i][j].red = img->data[i][j].green = img->data[i][j].blue = gray;
        }
}

void bmp24_brightness(t_bmp24 *img, int value) {
    for (int i = 0; i < img->height; i++)
        for (int j = 0; j < img->width; j++) {
            int r = img->data[i][j].red + value;
            int g = img->data[i][j].green + value;
            int b = img->data[i][j].blue + value;
            img->data[i][j].red   = (r > 255) ? 255 : (r < 0) ? 0 : r;
            img->data[i][j].green = (g > 255) ? 255 : (g < 0) ? 0 : g;
            img->data[i][j].blue  = (b > 255) ? 255 : (b < 0) ? 0 : b;
        }
}

// --- Convolution ---
t_pixel bmp24_convolution(t_bmp24 *img, int x, int y, float **kernel, int kernelSize) {
    int k = kernelSize / 2;
    float r = 0, g = 0, b = 0;
    for (int i = -k; i <= k; i++) {
        for (int j = -k; j <= k; j++) {
            int xi = x + i;
            int yj = y + j;
            if (xi >= 0 && xi < img->height && yj >= 0 && yj < img->width) {
                r += img->data[xi][yj].red   * kernel[i + k][j + k];
                g += img->data[xi][yj].green * kernel[i + k][j + k];
                b += img->data[xi][yj].blue  * kernel[i + k][j + k];
            }
        }
    }
    t_pixel px;
    px.red   = (r > 255) ? 255 : (r < 0) ? 0 : (uint8_t)r;
    px.green = (g > 255) ? 255 : (g < 0) ? 0 : (uint8_t)g;
    px.blue  = (b > 255) ? 255 : (b < 0) ? 0 : (uint8_t)b;
    return px;
}

void bmp24_applyFilter(t_bmp24 *img, float **kernel, int kernelSize) {
    t_pixel **newData = bmp24_allocateDataPixels(img->width, img->height);
    int k = kernelSize / 2;
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            if (i < k || i >= img->height - k || j < k || j >= img->width - k) {
                newData[i][j] = img->data[i][j]; // keep border unchanged
            } else {
                newData[i][j] = bmp24_convolution(img, i, j, kernel, kernelSize);
            }
        }
    }
    bmp24_freeDataPixels(img->data, img->height);
    img->data = newData;
}

// --- Filter kernels and wrappers ---
float **allocKernel(const float *flat, int size) {
    float **kernel = malloc(size * sizeof(float *));
    for (int i = 0; i < size; i++) {
        kernel[i] = malloc(size * sizeof(float));
        for (int j = 0; j < size; j++)
            kernel[i][j] = flat[i * size + j];
    }
    return kernel;
}
void freeKernel(float **kernel, int size) {
    for (int i = 0; i < size; i++) free(kernel[i]);
    free(kernel);
}

void bmp24_boxBlur(t_bmp24 *img) {
    float flat[9] = {1/9.0,1/9.0,1/9.0, 1/9.0,1/9.0,1/9.0, 1/9.0,1/9.0,1/9.0};
    float **kernel = allocKernel(flat, 3);
    bmp24_applyFilter(img, kernel, 3);
    freeKernel(kernel, 3);
}
void bmp24_gaussianBlur(t_bmp24 *img) {
    float flat[9] = {1,2,1, 2,4,2, 1,2,1};
    for (int i = 0; i < 9; i++) flat[i] /= 16.0f;
    float **kernel = allocKernel(flat, 3);
    bmp24_applyFilter(img, kernel, 3);
    freeKernel(kernel, 3);
}
void bmp24_outline(t_bmp24 *img) {
    float flat[9] = {-1,-1,-1, -1,8,-1, -1,-1,-1};
    float **kernel = allocKernel(flat, 3);
    bmp24_applyFilter(img, kernel, 3);
    freeKernel(kernel, 3);
}
void bmp24_emboss(t_bmp24 *img) {
    float flat[9] = {-2,-1,0, -1,1,1, 0,1,2};
    float **kernel = allocKernel(flat, 3);
    bmp24_applyFilter(img, kernel, 3);
    freeKernel(kernel, 3);
}
void bmp24_sharpen(t_bmp24 *img) {
    float flat[9] = {0,-1,0, -1,5,-1, 0,-1,0};
    float **kernel = allocKernel(flat, 3);
    bmp24_applyFilter(img, kernel, 3);
    freeKernel(kernel, 3);
}