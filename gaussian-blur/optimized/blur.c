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

void kernel(Image *outImage, Image *inImage, fImage *kernel)
{
    // implement basic kernel first
    int kernRadius = floor(kernel->width / 2);
    for (int c = 0; c < inImage->numChannels; c++) {
        for (int ih = 0; ih < inImage->height; ih++) {
            for (int iw = 0; iw < inImage->width; iw++) {
                for (int kh = 0; kh < kernel->height; kh++) {
                    int y = ih + kh - kernRadius;
                    for (int kw = 0; kw < kernel->width; kw++) {
                        int x = iw + kw - kernRadius;
                        if (x >= 0 && y >= 0
                                && x < inImage->width && y < inImage->height) {
                            outImage->data[c][ih*outImage->width + iw] +=
                                inImage->data[c][y*inImage->width + x] * kernel->data[0][kh*kernel->width + kw];
                        }
                    }
                }
            }
        }
    }
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
    // do multiple iterations to get a steady time
    int iterations = 50;
    unsigned long long start, end, sum = 0;

    // load in image to be blurred
    Image image;
    loadImage(&image, "image.png");

    // create output space
    Image outImage;
    outImage.numChannels = image.numChannels;
    outImage.height = image.height;
    outImage.width = image.width;
    outImage.data = (unsigned char**)malloc(outImage.numChannels * sizeof(unsigned char*));
    for (int i = 0; i < outImage.numChannels; i++) {
        outImage.data[i] = (unsigned char*)malloc(outImage.width * outImage.height * sizeof(unsigned char));
    }

    // generate gaussian filter: store it as a single channel image
    printf("Generating gaussian filter\n");
    fImage filter;
    generateGaussian(&filter, 2, 1.0f);

    printf("Performing blur...\n");
    // do the blur
    for (int i = 0; i < iterations; i++) {
        printf("Iteration %d/%d\n", i+1, iterations);
        // reset data
        for (int j = 0; j < outImage.numChannels; j++) {
            bzero(outImage.data[j], outImage.width * outImage.height * sizeof(unsigned char));
        }

        start = rdtsc();
        kernel(&outImage, &image, &filter);
        end = rdtsc();
        sum += (end - start);
    }

    printf("Saving blurred image\n");
    saveImage(&outImage, "newImage.png");

    // since we are using a 5x5 filter, there will be 25 fma operations per 
    // pixel, which amounts to 50 floating point ops per pixel
    int channels = outImage.numChannels;
    int height = outImage.height;
    int width = outImage.width;
    // (multiply+add) * (filterWidth * filterHeight) * (imageWidth * imageHeight) * imageChannels
    float flops = 2.0 * 5.0*5.0 * height * width * channels;
    // cycles / (cycles/second) = seconds
    float gseconds = (float)sum / (2.4);
    float gflops = flops / gseconds;

    printf("%f gflops\n", gflops);

    // free here
    // image
    for (int i = 0; i < image.numChannels; i++) {
        free(image.data[i]);
    }
    free(image.data);
    // filter - we know it's 1-D, so no loop
    free(filter.data[0]);
    free(filter.data);
    // out image
    for (int i = 0; i < outImage.numChannels; i++) {
        free(outImage.data[i]);
    }
    free(outImage.data);

    return 0;
}
