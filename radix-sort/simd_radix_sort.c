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


    static __m256i and_mask = _mm256_setr_epi32(15, 15, 15, 15, 15, 15, 15, 15);
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

    for (int i = 0; i < size; i += 32) {
        a = _mm256_setr_epi32((*arr)[i], (*arr)[i + 1], (*arr)[i + 2],
                              (*arr)[i + 3], (*arr)[i + 4], (*arr)[i + 5],
                              (*arr)[i + 6], (*arr)[i + 7]);
        b = _mm256_setr_epi32((*arr)[i + 8], (*arr)[i + 9], (*arr)[i + 10],
                              (*arr)[i + 11], (*arr)[i + 12], (*arr)[i + 13],
                              (*arr)[i + 14], (*arr)[i + 15]);
        c = _mm256_setr_epi32((*arr)[i + 16], (*arr)[i + 17], (*arr)[i + 18],
                              (*arr)[i + 19], (*arr)[i + 20], (*arr)[i + 21],
                              (*arr)[i + 22], (*arr)[i + 23]);
        d = _mm256_setr_epi32((*arr)[i + 24], (*arr)[i + 25], (*arr)[i + 26],
                              (*arr)[i + 27], (*arr)[i + 28], (*arr)[i + 29],
                              (*arr)[i + 30], (*arr)[i + 31]);

        a = _mm256_sra_epi32(a, shr);
        b = _mm256_sra_epi32(b, shr);
        c = _mm256_sra_epi32(c, shr);
        d = _mm256_sra_epi32(d, shr);

        a = _mm256_and_si256(a, and_mask);
        b = _mm256_and_si256(b, and_mask);
        c = _mm256_and_si256(c, and_mask);
        d = _mm256_and_si256(d, and_mask);

        res1 = (int*) &a;
        res2 = (int*) &b;
        res3 = (int*) &c;
        res4 = (int*) &d;

        count[res1[0]]++;
        count[res2[0]]++;
        count[res3[0]]++;
        count[res4[0]]++;
        count[res1[1]]++;
        count[res2[1]]++;
        count[res3[1]]++;
        count[res4[1]]++;
        count[res1[2]]++;
        count[res2[2]]++;
        count[res3[2]]++;
        count[res4[2]]++;
        count[res1[3]]++;
        count[res2[3]]++;
        count[res3[3]]++;
        count[res4[3]]++;
        count[res1[4]]++;
        count[res2[4]]++;
        count[res3[4]]++;
        count[res4[4]]++;
        count[res1[5]]++;
        count[res2[5]]++;
        count[res3[5]]++;
        count[res4[5]]++;
        count[res1[6]]++;
        count[res2[6]]++;
        count[res3[6]]++;
        count[res4[6]]++;
        count[res1[7]]++;
        count[res2[7]]++;
        count[res3[7]]++;
        count[res4[7]]++;
    }

    count[0] -= 1;
    for (int i = 1; i < 16; ++i) {
        count[i] += count[i - 1];
    }

    for (int i = size - 1; i >= 0; i -= 32) {
        a = _mm256_setr_epi32((*arr)[i], (*arr)[i - 1], (*arr)[i - 2],
                              (*arr)[i - 3], (*arr)[i - 4], (*arr)[i - 5],
                              (*arr)[i - 6], (*arr)[i - 7]);
        b = _mm256_setr_epi32((*arr)[i - 8], (*arr)[i - 9], (*arr)[i - 10],
                              (*arr)[i - 11], (*arr)[i - 12], (*arr)[i - 13],
                              (*arr)[i - 14], (*arr)[i - 15]);
        c = _mm256_setr_epi32((*arr)[i - 16], (*arr)[i - 17], (*arr)[i - 18],
                              (*arr)[i - 19], (*arr)[i - 20], (*arr)[i - 21],
                              (*arr)[i - 22], (*arr)[i - 23]);
        d = _mm256_setr_epi32((*arr)[i - 24], (*arr)[i - 25], (*arr)[i - 26],
                              (*arr)[i - 27], (*arr)[i - 28], (*arr)[i - 29],
                              (*arr)[i - 30], (*arr)[i - 31]);

        a = _mm256_sra_epi32(a, shr);
        b = _mm256_sra_epi32(b, shr);
        c = _mm256_sra_epi32(c, shr);
        d = _mm256_sra_epi32(d, shr);

        a = _mm256_and_si256(a, and_mask);
        b = _mm256_and_si256(b, and_mask);
        c = _mm256_and_si256(c, and_mask);
        d = _mm256_and_si256(d, and_mask);

        res1 = (int*) &a;
        res2 = (int*) &b;
        res3 = (int*) &c;
        res4 = (int*) &d;

        tempStorage[count[res1[0]]--] = (*arr)[i];
        tempStorage[count[res1[1]]--] = (*arr)[i - 1];
        tempStorage[count[res1[2]]--] = (*arr)[i - 2];
        tempStorage[count[res1[3]]--] = (*arr)[i - 3];
        tempStorage[count[res1[4]]--] = (*arr)[i - 4];
        tempStorage[count[res1[5]]--] = (*arr)[i - 5];
        tempStorage[count[res1[6]]--] = (*arr)[i - 6];
        tempStorage[count[res1[7]]--] = (*arr)[i - 7];
        tempStorage[count[res2[0]]--] = (*arr)[i - 8];
        tempStorage[count[res2[1]]--] = (*arr)[i - 9];
        tempStorage[count[res2[2]]--] = (*arr)[i - 10];
        tempStorage[count[res2[3]]--] = (*arr)[i - 11];
        tempStorage[count[res2[4]]--] = (*arr)[i - 12];
        tempStorage[count[res2[5]]--] = (*arr)[i - 13];
        tempStorage[count[res2[6]]--] = (*arr)[i - 14];
        tempStorage[count[res2[7]]--] = (*arr)[i - 15];
        tempStorage[count[res3[0]]--] = (*arr)[i - 16];
        tempStorage[count[res3[1]]--] = (*arr)[i - 17];
        tempStorage[count[res3[2]]--] = (*arr)[i - 18];
        tempStorage[count[res3[3]]--] = (*arr)[i - 19];
        tempStorage[count[res3[4]]--] = (*arr)[i - 20];
        tempStorage[count[res3[5]]--] = (*arr)[i - 21];
        tempStorage[count[res3[6]]--] = (*arr)[i - 22];
        tempStorage[count[res3[7]]--] = (*arr)[i - 23];
        tempStorage[count[res4[0]]--] = (*arr)[i - 24];
        tempStorage[count[res4[1]]--] = (*arr)[i - 25];
        tempStorage[count[res4[2]]--] = (*arr)[i - 26];
        tempStorage[count[res4[3]]--] = (*arr)[i - 27];
        tempStorage[count[res4[4]]--] = (*arr)[i - 28];
        tempStorage[count[res4[5]]--] = (*arr)[i - 29];
        tempStorage[count[res4[6]]--] = (*arr)[i - 30];
        tempStorage[count[res4[7]]--] = (*arr)[i - 31];
    }


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
