#include <stdio.h>
#include <stdlib.h>
#include "Fichiers h/bmp8.h"
#include "Fichiers h/bmp24.h"

int main() {
    int choice;
    char filename[256];
    t_bmp8 *image8 = NULL;
    t_bmp24 *image24 = NULL;

    printf("=== Image Processing Program ===\n");

    do {
        printf("\n--- Main Menu ---\n");
        printf("1. Load BMP 8-bit image\n");
        printf("2. Load BMP 24-bit image\n");
        printf("3. Apply Negative\n");
        printf("4. Adjust Brightness\n");
        printf("5. Threshold (8-bit only)\n");
        printf("6. Save Image\n");
        printf("7. Print Image Info\n");
        printf("8. Apply 24-bit Filter\n");
        printf("0. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                if (image8) bmp8_free(image8);
                printf("Enter filename to load: ");
                scanf("%s", filename);
                image8 = bmp8_loadImage(filename);
                if (image8) printf("8-bit image loaded successfully!\n");
                else printf("Failed to load 8-bit image.\n");
                break;
            case 2:
                if (image24) bmp24_free(image24);
                printf("Enter filename to load: ");
                scanf("%s", filename);
                image24 = bmp24_loadImage(filename);
                if (image24) printf("24-bit image loaded successfully!\n");
                else printf("Failed to load 24-bit image.\n");
                break;
            case 3:
                if (image8) bmp8_negative(image8);
                if (image24) bmp24_negative(image24);
                printf("Negative applied.\n");
                break;
            case 4: {
                int brightness;
                printf("Enter brightness value (-255 to 255): ");
                scanf("%d", &brightness);
                if (image8) bmp8_brightness(image8, brightness);
                if (image24) bmp24_brightness(image24, brightness);
                printf("Brightness adjusted.\n");
                break;
            }
            case 5:
                if (image8) {
                    int threshold;
                    printf("Enter threshold (0-255): ");
                    scanf("%d", &threshold);
                    bmp8_threshold(image8, threshold);
                    printf("Threshold applied.\n");
                } else {
                    printf("Only 8-bit images can be thresholded!\n");
                }
                break;
            case 6:
                printf("Enter filename to save: ");
                scanf("%s", filename);
                if (image8) bmp8_saveImage(filename, image8);
                if (image24) bmp24_saveImage(filename, image24);
                printf("Image saved.\n");
                break;
            case 7:
                if (image8) bmp8_printInfo(image8);
                if (image24) {
                    printf("\n--- Image Info ---\n");
                    printf("Width: %d px\n", image24->width);
                    printf("Height: %d px\n", image24->height);
                    printf("Color Depth: %d bits\n", image24->colorDepth);
                }
                break;
            case 8:
                if (image24) {
                    int fchoice;
                    printf("--- 24-bit Filters ---\n");
                    printf("1. Box Blur\n2. Gaussian Blur\n3. Outline\n4. Emboss\n5. Sharpen\n0. Cancel\nChoice: ");
                    scanf("%d", &fchoice);
                    switch (fchoice) {
                        case 1: bmp24_boxBlur(image24); printf("Box blur applied.\n"); break;
                        case 2: bmp24_gaussianBlur(image24); printf("Gaussian blur applied.\n"); break;
                        case 3: bmp24_outline(image24); printf("Outline filter applied.\n"); break;
                        case 4: bmp24_emboss(image24); printf("Emboss filter applied.\n"); break;
                        case 5: bmp24_sharpen(image24); printf("Sharpen filter applied.\n"); break;
                        default: printf("Cancelled.\n"); break;
                    }
                } else {
                    printf("Load a 24-bit image first!\n");
                }
                break;
            case 0:
                printf("Goodbye!\n");
                break;
            default:
                printf("Invalid choice.\n");
                break;
        }

    } while (choice != 0);

    if (image8) bmp8_free(image8);
    if (image24) bmp24_free(image24);

    return 0;
}