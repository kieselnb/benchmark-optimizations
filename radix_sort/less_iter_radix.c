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
    int count1[16], count2[16], count3[16],
        count4[16], count5[16], count6[16],
        count7[16], count8[16];
    int result1, result2, result3, result4, result5,
        result6, result7, result8;
    int  *tempStorage;
    posix_memalign((void**) &tempStorage, 32, sizeof(int)*size);

    for (int i = 0; i < 16; ++i) {
        count1[i] = 0;
        count2[i] = 0;
        count3[i] = 0;
        count4[i] = 0;
        count5[i] = 0;
        count6[i] = 0;
        count7[i] = 0;
        count8[i] = 0;
    }

    for (int i = 0; i < size; ++i) {
        result1 = ((*arr)[i]) & 0xf;
        count1[result1]++;
        result1 = ((*arr)[i] >> 4) & 0xf;
        count2[result1]++;
        result1 = ((*arr)[i] >> 8) & 0xf;
        count3[result1]++;
        result1 = ((*arr)[i] >> 12) & 0xf;
        count4[result1]++;
        result1 = ((*arr)[i] >> 16) & 0xf;
        count5[result1]++;
        result1 = ((*arr)[i] >> 20) & 0xf;
        count6[result1]++;
        result1 = ((*arr)[i] >> 24) & 0xf;
        count7[result1]++;
        result1 = ((*arr)[i] >> 28) & 0xf;
        count8[result1]++;
    }

    count1[0] -= 1;
    count2[0] -= 1;
    count3[0] -= 1;
    count4[0] -= 1;
    count5[0] -= 1;
    count6[0] -= 1;
    count7[0] -= 1;
    count8[0] -= 1;

    for (int i = 1; i < 16; ++i) {
        count1[i] += count1[i - 1];
        count2[i] += count2[i - 1];
        count3[i] += count3[i - 1];
        count4[i] += count4[i - 1];
        count5[i] += count5[i - 1];
        count6[i] += count6[i - 1];
        count7[i] += count7[i - 1];
        count8[i] += count8[i - 1];
    }

    for (int i = size - 1; i >= 0; --i) {
        result1 = ((*arr)[i]) & 0xf;
        tempStorage[count1[result1]] = (*arr)[i];
        count1[result1]--;
    }

    int *temp = (*arr);
    (*arr) = tempStorage; 
    tempStorage = temp;

    for (int i = size - 1; i >= 0; --i) {
        result1 = ((*arr)[i]) >> 4 & 0xf;
        tempStorage[count2[result1]] = (*arr)[i];
        count2[result1]--;
    }

    temp = (*arr);
    (*arr) = tempStorage;
    tempStorage = temp;

    for (int i = size - 1; i >= 0; --i) {
        result1 = ((*arr)[i]) >> 8 & 0xf;
        tempStorage[count3[result1]] = (*arr)[i];
        count3[result1]--;
    }

    temp = (*arr);
    (*arr) = tempStorage;
    tempStorage = temp;

    for (int i = size - 1; i >= 0; --i) {
        result1 = ((*arr)[i]) >> 12 & 0xf;
        tempStorage[count4[result1]] = (*arr)[i];
        count4[result1]--;
    }

    temp = (*arr);
    (*arr) = tempStorage;
    tempStorage = temp;

    for (int i = size - 1; i >= 0; --i) {
        result1 = ((*arr)[i]) >> 16 & 0xf;
        tempStorage[count5[result1]] = (*arr)[i];
        count5[result1]--;
    }

    temp = (*arr);
    (*arr) = tempStorage;
    tempStorage = temp;

    for (int i = size - 1; i >= 0; --i) {
        result1 = ((*arr)[i]) >> 20 & 0xf;
        tempStorage[count6[result1]] = (*arr)[i];
        count6[result1]--;
    }

    temp = (*arr);
    (*arr) = tempStorage;
    tempStorage = temp;

    for (int i = size - 1; i >= 0; --i) {
        result1 = ((*arr)[i]) >> 24 & 0xf;
        tempStorage[count7[result1]] = (*arr)[i];
        count7[result1]--;
    }

    temp = (*arr);
    (*arr) = tempStorage;
    tempStorage = temp;
    
    for (int i = size - 1; i >= 0; --i) {
        result1 = ((*arr)[i]) >> 28 & 0xf;
        tempStorage[count8[result1]] = (*arr)[i];
        count8[result1]--;
    }

    temp = (*arr);
    (*arr) = tempStorage; 
    free(temp);
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
