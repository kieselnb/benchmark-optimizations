/**
 * @file blur.c
 * @author Nick Kiesel (nkiesel)
 *
 * This file contains the optimized implementation of Gaussian blur for the
 * Intel Haswell CPU architecture
 */

#include <stdio.h>
#include <assert.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../third-party/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../third-party/stb_image_write.h"

typedef struct Image {
    int numChannels;
    int height;
    int width;
    unsigned char **data;
} Image;

static __inline__ unsigned long long rdtsc(void)
{
  unsigned hi, lo;
  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
  return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}

void kernel()
{
}

void loadImage(Image* image, const char *filename)
{
    int x, y, n;
    unsigned char *imageData = stbi_load(filename, &x, &y, &n, 0);

    // store data in custom container
    image->numChannels = n;
    image->height = y;
    image->width = x;

    if (n > 3) {
        if (n == 4) {
            printf("Ignoring transparency layer\n");
            image->numChannels = 3;
        }
        else {
            printf("I don't know how to handle %d channels, bailing out\n", n);
        }
    }

    image->data = (unsigned char**)malloc(image->numChannels * sizeof(unsigned char*));
    for (int i = 0; i < image->numChannels; i++) {
        image->data[i] = (unsigned char*)malloc(x*y * sizeof(unsigned char));
        for (int j = 0; j < y; j++) {
            for (int k = 0; k < x; k++) {
                image->data[i][j*x + k] = imageData[n*(j*x + k) + i];
            }
        }
    }
    
    printf("Image details: %dx%d, %d channels\n", image->width, image->height,
            image->numChannels);
    free(imageData);
}

void saveImage(Image* image, const char *filename)
{
    assert(image->numChannels == 3);

    unsigned char *imageData;
    imageData = (unsigned char*)malloc(image->height*image->width*3 * sizeof(unsigned char));

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            imageData[3*(y*image->width + x)] = image->data[0][y*image->width + x];
            imageData[3*(y*image->width + x) + 1] = image->data[1][y*image->width + x];
            imageData[3*(y*image->width + x) + 2] = image->data[2][y*image->width + x];
        }
    }

    stbi_write_png(filename, image->width, image->height, 3, imageData,
            3*image->width*sizeof(unsigned char));

    free(imageData);
}

int main(int argc, char *argv[])
{
    printf("Hello, Earthlings\n");
    Image image;
    loadImage(&image, "image.png");

    saveImage(&image, "newImage.png");

    // free here
    for (int i = 0; i < image.numChannels; i++) {
        free(image.data[i]);
    }
    free(image.data);

    return 0;
}
