/**
 * @file blur.c
 * @author Nick Kiesel (nkiesel)
 *
 * This file contains the optimized implementation of Gaussian blur for the
 * Intel Haswell CPU architecture
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../third-party/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../third-party/stb_image_write.h"

static __inline__ unsigned long long rdtsc(void)
{
  unsigned hi, lo;
  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
  return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}

/**
 * Stores an image in row-major format. Each color channel is accessed from
 * the top level (e.g. data[channel][pixel]) so that pixels of the same channel
 * are contiguous in memory.
 */
typedef struct Image {
    int numChannels;
    int height;
    int width;
    unsigned char **data;
} Image;

typedef struct fImage {
    int numChannels;
    int height;
    int width;
    float **data;
} fImage;

void kernel()
{
    printf("not implemented yet\n");
}

/**
 * Loads a png image from disk. If the image has 4 channels (i.e. is RGBA),
 * the transparency channel is ignored. If the image has more than 4 channels,
 * an error is thrown and the program exits.
 */
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
            printf("loadImage: Ignoring transparency layer\n");
            image->numChannels = 3;
        }
        else {
            printf("I don't know how to handle %d channels, bailing out\n", n);
            exit(EXIT_FAILURE);
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

/**
 * Saves png image to disk. Must be a 3-channel image (i.e. RGB).
 */
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

/**
 * Generates a Gaussian filter of the desired radius. Radius is the number of
 * rows/columns on either side of the center. E.g. if radius is 2, then a 5x5
 * mask is made (2 rows/columns in each direction). If radius is 4, then
 * a 9x9 mask is made.
 *
 * This is a once-and-done event, so no optimizations here.
 */
void generateGaussian(fImage *filter, int radius, float sigma)
{
    filter->numChannels = 1;
    filter->height = 2*radius + 1;
    filter->width = 2*radius + 1;

    filter->data = (float**)malloc(sizeof(float*));
    filter->data[0] = (float*)malloc(filter->height*filter->width*sizeof(float));

    float twoSigmaSqrd = 2.0*sigma*sigma;
    float sum = 0.0;
    for (int i = 0; i < filter->height; i++) {
        for (int j = 0; j < filter->width; j++) {
            filter->data[0][i*filter->width + j] =
                exp(-(i*i + j*j)/twoSigmaSqrd) / (M_PI * twoSigmaSqrd);
            sum += filter->data[0][i*filter->width + j];
        }
    }

    // normalize
    for (int i = 0; i < filter->height; i++) {
        for (int j = 0; j < filter->width; j++) {
            filter->data[0][i*filter->width + j] /= sum;
        }
    }
}

int main(int argc, char *argv[])
{
    // load in image to be blurred
    Image image;
    loadImage(&image, "image.png");

    // generate gaussian filter: store it as a single channel image
    fImage filter;
    generateGaussian(&filter, 2, 1.0f);

    saveImage(&image, "newImage.png");

    // free here
    // image
    for (int i = 0; i < image.numChannels; i++) {
        free(image.data[i]);
    }
    free(image.data);
    // filter - we know it's 1-D, so no loop
    free(filter.data[0]);
    free(filter.data);

    return 0;
}
