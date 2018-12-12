#include <iostream>
#include <vector>
#include <assert.h>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "../third-party/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../third-party/stb_image_write.h"

static __inline__ unsigned long long rdtsc(void) {
  unsigned hi, lo;
  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
  return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}

using namespace std;

typedef vector<double> Array;
typedef vector<Array> Matrix;
typedef vector<Matrix> Image;

Matrix getGaussian(int height, int width, double sigma)
{
    Matrix kernel(height, Array(width));
    double sum=0.0;
    int i,j;

    for (i=0 ; i<height ; i++) {
        for (j=0 ; j<width ; j++) {
            kernel[i][j] = exp(-(i*i+j*j)/(2*sigma*sigma))/(2*M_PI*sigma*sigma);
            sum += kernel[i][j];
        }
    }

    for (i=0 ; i<height ; i++) {
        for (j=0 ; j<width ; j++) {
            kernel[i][j] /= sum;
        }
    }

    return kernel;
}

Image loadImage(const char *filename)
{
    int x, y, n;
    unsigned char *imageData = stbi_load(filename, &x, &y, &n, 0);
    Image imageMatrix(3, Matrix(y, Array(x)));

    int h,w;
    for (h=0 ; h<y ; h++) {
        for (w=0 ; w<x ; w++) {
            imageMatrix[0][h][w] = imageData[n*(h*x + w)];
            imageMatrix[1][h][w] = imageData[n*(h*x + w) + 1];
            imageMatrix[2][h][w] = imageData[n*(h*x + w) + 2];
        }
    }

    stbi_image_free(imageData);
    return imageMatrix;
}

void saveImage(Image &image, const char *filename)
{
    assert(image.size()==3);

    int height = image[0].size();
    int width = image[0][0].size();
    int x,y;

    unsigned char *imageData;
    imageData = (unsigned char*)malloc(height*width*3 * sizeof(unsigned char));

    for (y=0 ; y<height ; y++) {
        for (x=0 ; x<width ; x++) {
            imageData[3*(y*width + x)] = image[0][y][x];
            imageData[3*(y*width + x) + 1] = image[1][y][x];
            imageData[3*(y*width + x) + 2] = image[2][y][x];
        }
    }
    stbi_write_png(filename, width, height, 3, imageData,
            3*width*sizeof(unsigned char));

    free(imageData);
}

Image applyFilter(Image &image, Matrix &filter){
    assert(image.size()==3 && filter.size()!=0);

    int height = image[0].size();
    int width = image[0][0].size();
    int filterHeight = filter.size();
    int filterWidth = filter[0].size();
    int newImageHeight = height-filterHeight+1;
    int newImageWidth = width-filterWidth+1;
    int d,i,j,h,w;

    Image newImage(3, Matrix(newImageHeight, Array(newImageWidth)));

    for (d=0 ; d<3 ; d++) {
        for (i=0 ; i<newImageHeight ; i++) {
            for (j=0 ; j<newImageWidth ; j++) {
                for (h=i ; h<i+filterHeight ; h++) {
                    for (w=j ; w<j+filterWidth ; w++) {
                        newImage[d][i][j] += filter[h-i][w-j]*image[d][h][w];
                    }
                }
            }
        }
    }

    return newImage;
}

Image applyFilter(Image &image, Matrix &filter, int times)
{
    Image newImage = image;
    for(int i=0 ; i<times ; i++) {
        newImage = applyFilter(newImage, filter);
    }
    return newImage;
}

int main(int argc, char* argv[])
{
    unsigned long long sum = 0, start, end;
    int numIterations = atoi(argv[1]);
    int filterRadius = atoi(argv[2]);

    int filterWidth = (2 * filterRadius) + 1;

    cerr << "Loading image..." << endl;
    Image image = loadImage("image.png");
    Image newImage;

    for (int i = 0; i < numIterations; i++) {
        Matrix filter = getGaussian(filterWidth, filterWidth, 10.0);

        start = rdtsc();
        newImage = applyFilter(image, filter);
        end = rdtsc();
        sum += (end - start);
    }
    float floatSum = ((float)sum / (float)numIterations) * (3.2 / 2.4);

    cerr << "Saving image..." << endl;
    saveImage(newImage, "newImage.png");
    cerr << "Done!" << endl;

    fprintf(stdout, "%f\n", floatSum);
    cerr << "Took " << floatSum << " clock cycles on average." << endl;
    
    // since we are using a 5x5 filter, there will be 25 fma operations per 
    // pixel, which amounts to 50 floating point ops per pixel
    int channels = image.size();
    int height = image[0].size();
    int width = image[0][0].size();
    // (multiply+add) * (filterWidth * filterHeight) * (imageWidth * imageHeight) * imageChannels
    float flops = 2.0 * 5.0*5.0 * height * width * channels;
    // cycles / (cycles/second) = seconds
    float gseconds = (float)sum / (2.4);
    float gflops = flops / gseconds;

    cerr << "flops: " << flops << endl;
    cerr << "gseconds: " << gseconds << endl;
    cerr << "gflops: " << gflops << endl;
}
