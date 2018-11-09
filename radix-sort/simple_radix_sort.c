#include <stdio.h>
#include <stdlib.h>
#include <time.h>

unsigned long long rdtsc()
{
    unsigned a, d;
    __asm__ volatile("rdtsc" : "=a" (a), "=d" (d));
    return ((unsigned long long) a) | (((unsigned long long) d) << 32);
}


void countSort(int** arr, int shift, int size)
{
    int count1[16], result1, result2, result3, result4;
    int  *tempStorage;
    posix_memalign((void**) &tempStorage, 32, sizeof(int)*size);

    for (int i = 0; i < 16; ++i) {
        count1[i] = 0;
    }

    for (int i = 0; i < size; i += 4) {
        result1 = ((*arr)[i] >> shift) & 0xf;
        result2 = ((*arr)[i + 1] >> shift) & 0xf;
        count1[result1]++;
        result3 = ((*arr)[i + 2] >> shift) & 0xf;
        count1[result2]++;
        result4 = ((*arr)[i + 3] >> shift) & 0xf;
        count1[result3]++;
        count1[result4]++;
    }

    count1[0] -= 1;
    for (int i = 1; i < 16; ++i) {
        count1[i] += count1[i - 1];
    }

    for (int i = size - 1; i >= 0; --i) {
        result1 = ((*arr)[i]) >> shift & 0xf;
        tempStorage[count1[result1]] = (*arr)[i];
        count1[result1]--;
    }

    int* freeMe = (*arr);
    (*arr) = tempStorage;
    free(freeMe);
}


void radixSort(int** arr, int size)
{
    int shift = 4;
    while (shift < 32) {
        countSort(arr, shift, size);
        shift += 4;
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
        radixSort(&arr, size);
        et = rdtsc();

        sum += (et - st);
    }

    printArray(f, arr, size);
    printf("Execution time: %lf [cycles]\n", (double) (sum / (1.0 * runs)));
    free(arr);
    return (0);
}
