#include <stdio.h>
#include <stdlib.h>
#include <time.h>

unsigned long long rdtsc()
{
    unsigned long long int x;
    unsigned a, d;
    __asm__ volatile("rdtsc" : "=a" (a), "=d" (d));
    return ((unsigned long long) a) | (((unsigned long long) d) << 32);
}


bool countSort(int* arr, int shift, int size)
{
    int countZeros = 0;
    int countOnes = 0;
    int countTwos = 0;
    int countThrees = 0;

    int *zeros, *ones, *twos, *threes;
    posix_memalign((void**) &zeros, 32, sizeof(int)*size);
    posix_memalign((void**) &ones, 32, sizeof(int)*size);
    posix_memalign((void**) &twos, 32, sizeof(int)*size);
    posix_memalign((void**) &threes, 32, sizeof(int)*size);

    for (int i = 0; i < size; ++i) {
        int result = (arr[i] >> shift) & 0x11;
        switch (result) {
        case 0 :
            zeros[countZeros++] = arr[i];
            break;
        case 1 :
            ones[countOnes++] = arr[i];
            break;
        case 2 :
            twos[countTwos++] = arr[i];
            break;
        case 3 :
            threes[countThrees++] = arr[i];
            break;
        }
    }

    int idx = 0;
    for (; idx < countZeros; ++idx) {
        arr[idx] = zeros[idx];
    }
    
    int c = 0;
    while (c < countOnes) {
        arr[idx++] = ones[c++];
    }

    c = 0;
     
    while (c < countTwos) {
        arr[idx++] = twos[c++];
    }
   
    c = 0;
    while(idx < size)
        arr[idx++] =  ones[c++];

    free(zeros);
    free(ones);
    free(twos);
    free(threes);
    return (true);
}


void radixSort(int* arr, int size)
{
    int shift = 0;

    while (shift < 32) {
        countSort(arr, shift, size);
        shift += 2;
    }
}


void printArray(FILE* f, int* arr, int size)
{
    for (int i = 0; i < size; ++i) {
        fprintf(f, "%d ", arr[i]);
    }

    fprintf(f, "\n");
}


int main(int argc, char** argv)
{
    int size = atoi(argv[1]);
    int runs = atoi(argv[2]);

    int *arr;
    posix_memalign((void**) &arr, 32, sizeof(int)*size);
    srand(time(0));

    for (int i = 0 ; i < size; ++i) {
        arr[i] = rand();
    }

    FILE* f = fopen("result.txt", "w");
    printArray(f, arr, size);
    unsigned long long st, et, sum = 0;

    for (int i = 1; i <= runs; ++i) {
        st = rdtsc();
        radixSort(arr, size);
        et = rdtsc();

        sum += (et - st);
    }

    printArray(f, arr, size);
    printf("Execution time: %lf [cycles]\n", (double) (sum / (1.0 * runs)));
    free(arr);
    return (0);
}
