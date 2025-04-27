#include stdio.h
#include stdlib.h
#include bmp8.h
#include bmp24.h
#include filters.h
#include histogram.h

int main() {
    int choice;
    char filename[256];
    t_bmp8 image8 = NULL;
    t_bmp24 image24 = NULL;

    printf(=== Image Processing Program ===n);

    do {
        printf(n--- Main Menu ---n);
        printf(1. Load BMP 8-bit imagen);
        printf(2. Load BMP 24-bit imagen);
        printf(3. Apply Negativen);
        printf(4. Adjust Brightnessn);
        printf(5. Threshold (8-bit only)n);
        printf(6. Apply Convolution Filter (8-bit only)n);
        printf(7. Histogram Equalization (8-bit only)n);
        printf(8. Save Imagen);
        printf(9. Print Image Infon);
        printf(0. Exitn);
        printf(Choice );
        scanf(%d, &choice);

        switch (choice) {
            case 1
                if (image8) bmp8_free(image8);
                printf(Enter filename to load );
                scanf(%s, filename);
                image8 = bmp8_loadImage(filename);
                if (image8) printf(8-bit image loaded successfully!n);
                break;
            case 2
                if (image24) bmp24_free(image24);
                printf(Enter filename to load );
                scanf(%s, filename);
                image24 = bmp24_loadImage(filename);
                if (image24) printf(24-bit image loaded successfully!n);
                break;
            case 3
                if (image8) bmp8_negative(image8);
                if (image24) bmp24_negative(image24);
                printf(Negative applied.n);
                break;
            case 4 {
                int brightness;
                printf(Enter brightness value (-255 to 255) );
                scanf(%d, &brightness);
                if (image8) bmp8_brightness(image8, brightness);
                if (image24) bmp24_brightness(image24, brightness);
                printf(Brightness adjusted.n);
                break;
            }
            case 5
                if (image8) {
                    int threshold;
                    printf(Enter threshold (0-255) );
                    scanf(%d, &threshold);
                    bmp8_threshold(image8, threshold);
                    printf(Threshold applied.n);
                } else {
                    printf(Only 8-bit images can be thresholded!n);
                }
                break;
            case 6
                if (image8) {
                    printf(Select filtern);
                    printf(1. Box Blurn2. Gaussian Blurn3. Outlinen4. Embossn5. Sharpenn);
                    int filterChoice;
                    scanf(%d, &filterChoice);

                    float kernel = NULL;
                    switch (filterChoice) {
                        case 1 kernel = createBoxBlurKernel(); break;
                        case 2 kernel = createGaussianBlurKernel(); break;
                        case 3 kernel = createOutlineKernel(); break;
                        case 4 kernel = createEmbossKernel(); break;
                        case 5 kernel = createSharpenKernel(); break;
                        default printf(Invalid filter.n); break;
                    }

                    if (kernel) {
                        bmp8_applyFilter(image8, kernel, 3);
                        freeKernel(kernel, 3);
                        printf(Filter applied.n);
                    }
                } else {
                    printf(Only 8-bit images can be filtered!n);
                }
                break;
            case 7
                if (image8) {
                    bmp8_equalize(image8);
                    printf(Histogram equalization applied.n);
                } else {
                    printf(Only 8-bit images can be equalized!n);
                }
                break;
            case 8
                printf(Enter filename to save );
                scanf(%s, filename);
                if (image8) bmp8_saveImage(filename, image8);
                if (image24) bmp24_saveImage(filename, image24);
                printf(Image saved.n);
                break;
            case 9
                if (image8) bmp8_printInfo(image8);
                if (image24) {
                    printf(n--- Image Info ---n);
                    printf(Width %d pxn, image24-width);
                    printf(Height %d pxn, image24-height);
                    printf(Color Depth %d bitsn, image24-colorDepth);
                }
                break;
            case 0
                printf(Goodbye!n);
                break;
            default
                printf(Invalid choice.n);
                break;
        }

    } while (choice != 0);

    if (image8) bmp8_free(image8);
    if (image24) bmp24_free(image24);

    return 0;
}