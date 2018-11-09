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


void countSort(int** arr, int shift, int size)
{
    int count[16], result;
    int count_copy[16];

    for (int i = 0; i < 16; ++i) {
        count[i] = 0;
    }

    for (int i = 0; i < size; ++i) {
        result = ((*arr)[i] >> shift) & 0xf;
        count[result]++;
    }

    int curr_sum = 0;
    int prev_sum = 0;
    for (int i = 0; i < 16; ++i) {
        curr_sum += count[i];
        count[i] = prev_sum;
        prev_sum = curr_sum;
        count_copy[i] = count[i];
    }

    int curr_block = 0;
    int i = 0;
    int pos = 0;
    int temp = 0;
    while (curr_block < 15) {
        if (i >= count[curr_block + 1]) {
            curr_block += 1;
            continue;
        }
        result = ((*arr)[i]) >> shift & 0xf;
        if (result == curr_block) {
            ++i;
            continue;
        }
        pos = count_copy[result]++;
        temp = ((*arr)[pos]);
        ((*arr)[pos]) = (*arr)[i];
        (*arr)[i] = temp;
    }
}


void radixSort(int** arr, int size)
{
    int shift = 0;
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
    posix_memalign((void**) &arr, 64, sizeof(int)*size);
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
