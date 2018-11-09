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


void countSort(int** arr, int size)
{
    int count1[256], count2[256], count3[256], count4[256], result1, result2, result3, result4;
    int  *tempStorage;
    posix_memalign((void**) &tempStorage, 32, sizeof(int)*size);

    for (int i = 0; i < 255; ++i) {
        count1[i] = 0;
        count2[i] = 0;
        count3[i] = 0;
        count4[i] = 0;
    }

    for (int i = 0; i < size; ++i) {
        result1 = ((*arr)[i]) & 0xff;
        result2 = ((*arr)[i] >> 8) & 0xff;
        result3 = ((*arr)[i] >> 16) & 0xff;
        result4 = ((*arr)[i] >> 24) & 0xff;
        count1[result1]++;
        count2[result2]++;
        count3[result3]++;
        count4[result4]++;
    }

    count1[0] -= 1;
    count2[0] -= 1;
    count3[0] -= 1;
    count4[0] -= 1;

    for (int i = 1; i < 256; ++i) {
        count1[i] += count1[i - 1];
        count2[i] += count2[i - 1];
        count3[i] += count3[i - 1];
        count4[i] += count4[i - 1];
    }

    for (int i = size - 1; i >= 0; --i) {
        result1 = ((*arr)[i]) & 0xff;
        tempStorage[count1[result1]] = (*arr)[i];
        count1[result1]--;
    }

/*    for (int i = size - 1; i >= 0; --i) {
        result1 = (tempStorage[i] >> 8) & 0xff;
        (*arr)[count2[result1]] = tempStorage[i];
        count2[result1]--;
    } */

/*    for (int i = size - 1; i >= 0; --i) {
        result1 = ((*arr)[i] >> 16) & 0xff;
        tempStorage[count3[result1]] = (*arr)[i];
        count3[result1]--;
    }

    for (int i = size - 1; i >= 0; --i) {
        result1 = (tempStorage[i] >> 24) & 0xff;
        (*arr)[count4[result1]] = tempStorage[i];
        count4[result1]--;
    }*/

    int* freeMe = (*arr);
    (*arr) = tempStorage;
    free(freeMe);
}


void radixSort(int** arr, int size)
{
    countSort(arr, size);
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
