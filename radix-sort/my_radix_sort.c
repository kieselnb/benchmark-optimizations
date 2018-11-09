#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <immintrin.h>


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
    int  *tempStorage, *res1, *res2, *res3, *res4;
    posix_memalign((void**) &tempStorage, 32, sizeof(int)*size);


    __m256i and_mask = _mm256_setr_epi32(15, 15, 15, 15, 15, 15, 15, 15);
    __m128i shr = _mm_setr_epi32(shift, shift, shift, shift);
    __m256i a, b, c, d;

    count[0] = 0;
    count[1] = 0;
    count[2] = 0;
    count[3] = 0;
    count[4] = 0;
    count[5] = 0;
    count[6] = 0;
    count[7] = 0;
    count[8] = 0;
    count[9] = 0;
    count[10] = 0;
    count[11] = 0;
    count[12] = 0;
    count[13] = 0;
    count[14] = 0;
    count[15] = 0;

    for (int i = 0; i < size; i += 8) {
        a = _mm256_setr_epi32((*arr)[i], (*arr)[i + 1], (*arr)[i + 2],
                              (*arr)[i + 3], (*arr)[i + 4], (*arr)[i + 5],
                              (*arr)[i + 6], (*arr)[i + 7]);
        a = _mm256_sra_epi32(a, shr);
        a = _mm256_and_si256(a, and_mask);

        res1 = (int*) &a;

        count[res1[0]]++;
        count[res1[1]]++;
        count[res1[2]]++;
        count[res1[3]]++;
        count[res1[4]]++;
        count[res1[5]]++;
        count[res1[6]]++;
        count[res1[7]]++;
    }

    count[0] -= 1;
    for (int i = 1; i < 16; ++i) {
        count[i] += count[i - 1];
    }

    for (int i = size - 1; i >= 0; i -= 8) {
        a = _mm256_setr_epi32((*arr)[i], (*arr)[i - 1], (*arr)[i - 2],
                              (*arr)[i - 3], (*arr)[i - 4], (*arr)[i - 5],
                              (*arr)[i - 6], (*arr)[i - 7]);

        a = _mm256_sra_epi32(a, shr);

        a = _mm256_and_si256(a, and_mask);

        res1 = (int*) &a;

        tempStorage[count[res1[0]]--] = (*arr)[i];
        tempStorage[count[res1[1]]--] = (*arr)[i - 1];
        tempStorage[count[res1[2]]--] = (*arr)[i - 2];
        tempStorage[count[res1[3]]--] = (*arr)[i - 3];
        tempStorage[count[res1[4]]--] = (*arr)[i - 4];
        tempStorage[count[res1[5]]--] = (*arr)[i - 5];
        tempStorage[count[res1[6]]--] = (*arr)[i - 6];
        tempStorage[count[res1[7]]--] = (*arr)[i - 7];
    }

//    for (int i = size - 1; i >= 0; i--) {
//        result = ((*arr)[i] >> shift) & 0xf;
//        tempStorage[count[result]--] = (*arr)[i];
//    }

    int *freeMe = *(arr);
    *(arr) = tempStorage;
    free(freeMe);
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
