#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <immintrin.h>

unsigned long long rdtsc()
{
    unsigned a, d;
    __asm__ volatile("rdtsc" : "=a" (a), "=d" (d));
    return ((unsigned long long) a) | (((unsigned long long) d) << 32);
}

int *t1, *t2, *t3, *t4;

void fast_sort_kernel(int **arr, int shift, int size)
{
    int *mem_ptr[16];

    // Initialization step

    mem_ptr[0]  = t1; mem_ptr[1]  = t1 + 256; mem_ptr[2]  = t1 + 512; mem_ptr[3]  = t1 + 768;
    mem_ptr[4]  = t2; mem_ptr[5]  = t2 + 256; mem_ptr[6]  = t2 + 512; mem_ptr[7]  = t2 + 768;
    mem_ptr[8]  = t3; mem_ptr[9]  = t3 + 256; mem_ptr[10] = t3 + 512; mem_ptr[11] = t3 + 768;
    mem_ptr[12] = t4; mem_ptr[13] = t4 + 256; mem_ptr[14] = t4 + 512; mem_ptr[15] = t4 + 768;

    //////////////////////////////////////////////////////////
    
    __m256i a, b;
    static __m256i and_mask = _mm256_setr_epi32(15, 15, 15, 15, 15, 15, 15, 15);
    int r1, r2, r3, r4, r5, r6, r7, r8;
    int d1, d2, d3, d4, d5, d6, d7, d8;

    for (int i = 0; i < size / 8; ++i) {
        b = _mm256_loadu_si256((__m256i*)(*arr) + i);
        a = _mm256_srli_epi32(b, shift);
        a = _mm256_and_si256(a, and_mask);
        
        r1 = _mm256_extract_epi32(a, 0);
        d1 = _mm256_extract_epi32(b, 0);
        r2 = _mm256_extract_epi32(a, 1);
        d2 = _mm256_extract_epi32(b, 1);
        *(mem_ptr[r1]) = d1;
        mem_ptr[r1] += 1;

        r3 = _mm256_extract_epi32(a, 2);
        d3 = _mm256_extract_epi32(b, 2);
        *(mem_ptr[r2]) = d2;
        mem_ptr[r2] += 1;

        r4 = _mm256_extract_epi32(a, 3);
        d4 = _mm256_extract_epi32(b, 3);
        *(mem_ptr[r3]) = d3;
        mem_ptr[r3] += 1;

        r5 = _mm256_extract_epi32(a, 4);
        d5 = _mm256_extract_epi32(b, 4);
        *(mem_ptr[r4]) = d4;
        mem_ptr[r4] += 1;


        r6 = _mm256_extract_epi32(a, 5);
        d6 = _mm256_extract_epi32(b, 5);
        *(mem_ptr[r5]) = d5;
        mem_ptr[r5] += 1;

        r7 = _mm256_extract_epi32(a, 6);
        d7 = _mm256_extract_epi32(b, 6);
        *(mem_ptr[r6]) = d6;
        mem_ptr[r6] += 1;

        r8 = _mm256_extract_epi32(a, 7);
        d8 = _mm256_extract_epi32(b, 7);
        *(mem_ptr[r7]) = d7;
        mem_ptr[r7] += 1;

        *(mem_ptr[r8]) = d8;
        mem_ptr[r8] += 1;
    } 

    // Work on way 1
    int count = (mem_ptr[0] - t1);
    int *put = t1;
    for (int i = 0; i <= count / 8; ++i) {
        b = _mm256_loadu_si256((__m256i *) put + i);
        _mm256_storeu_si256((__m256i *)(*arr) + i, b);
    }

    int *pos_array = (*arr) + count;
    put = t1 + 256;
    count = (mem_ptr[1] - (t1 + 256));
    for (int i = 0; i <= count / 8; ++i) {
        b = _mm256_loadu_si256((__m256i *) put + i);
        _mm256_storeu_si256((__m256i *)(pos_array) + i, b);
    }

    pos_array += count;
    put = t1 + 512;
    count = (mem_ptr[2] - (t1 + 512));
    for (int i = 0; i <= count / 8; ++i) {
        b = _mm256_loadu_si256((__m256i *) put + i);
        _mm256_storeu_si256((__m256i *)(pos_array) + i, b);
    }

    pos_array += count;
    put = t1 + 768;
    count = (mem_ptr[3] - (t1 + 768));
    for (int i = 0; i <= count / 8; ++i) {
        b = _mm256_loadu_si256((__m256i *) put + i);
        _mm256_storeu_si256((__m256i *)(pos_array) + i, b);
    }

    // Work on Way2
    pos_array += count;
    put = t2;
    count = (mem_ptr[4] - (t2));
    for (int i = 0; i <= count / 8; ++i) {
        b = _mm256_loadu_si256((__m256i *) put + i);
        _mm256_storeu_si256((__m256i *)(pos_array) + i, b);
    }

    pos_array += count;
    put = t2 + 256;
    count = (mem_ptr[5] - (t2 + 256));
    for (int i = 0; i <= count / 8; ++i) {
        b = _mm256_loadu_si256((__m256i *) put + i);
        _mm256_storeu_si256((__m256i *)(pos_array) + i, b);
    }

    pos_array += count;
    put = t2 + 512;
    count = (mem_ptr[6] - (t2 + 512));
    for (int i = 0; i <= count / 8; ++i) {
        b = _mm256_loadu_si256((__m256i *) put + i);
        _mm256_storeu_si256((__m256i *)(pos_array) + i, b);
    }

    pos_array += count;
    put = t2 + 768;
    count = (mem_ptr[7] - (t2 + 768));
    for (int i = 0; i <= count / 8; ++i) {
        b = _mm256_loadu_si256((__m256i *) put + i);
        _mm256_storeu_si256((__m256i *)(pos_array) + i, b);
    }

    // Work on way 3
    pos_array += count;
    put = t3;
    count = (mem_ptr[8] - (t3));
    for (int i = 0; i <= count / 8; ++i) {
        b = _mm256_loadu_si256((__m256i *) put + i);
        _mm256_storeu_si256((__m256i *)(pos_array) + i, b);
    }

    pos_array += count;
    put = t3 + 256;
    count = (mem_ptr[9] - (t3 + 256));
    for (int i = 0; i <= count / 8; ++i) {
        b = _mm256_loadu_si256((__m256i *) put + i);
        _mm256_storeu_si256((__m256i *)(pos_array) + i, b);
    }

    pos_array += count;
    put = t3 + 512;
    count = (mem_ptr[10] - (t3 + 512));
    for (int i = 0; i <= count / 8; ++i) {
        b = _mm256_loadu_si256((__m256i *) put + i);
        _mm256_storeu_si256((__m256i *)(pos_array) + i, b);
    }

    pos_array += count;
    put = t3 + 768;
    count = (mem_ptr[11] - (t3 + 768));
    for (int i = 0; i <= count / 8; ++i) {
        b = _mm256_loadu_si256((__m256i *) put + i);
        _mm256_storeu_si256((__m256i *)(pos_array) + i, b);
    }

    // Work on way 4
    pos_array += count;
    put = t4;
    count = (mem_ptr[12] - (t4));
    for (int i = 0; i <= count / 8; ++i) {
        b = _mm256_loadu_si256((__m256i *) put + i);
        _mm256_storeu_si256((__m256i *)(pos_array) + i, b);
    }

    pos_array += count;
    put = t4 + 256;
    count = (mem_ptr[13] - (t4 + 256));
    for (int i = 0; i <= count / 8; ++i) {
        b = _mm256_loadu_si256((__m256i *) put + i);
        _mm256_storeu_si256((__m256i *)(pos_array) + i, b);
    }

    pos_array += count;
    put = t4 + 512;
    count = (mem_ptr[14] - (t4 + 512));
    for (int i = 0; i <= count / 8; ++i) {
        b = _mm256_loadu_si256((__m256i *) put + i);
        _mm256_storeu_si256((__m256i *)(pos_array) + i, b);
    }

    pos_array += count;
    put = t4 + 768;
    count = (mem_ptr[15] - (t4 + 768));
    for (int i = 0; i <= count / 8; ++i) {
        b = _mm256_loadu_si256((__m256i *) put + i);
        _mm256_storeu_si256((__m256i *)(pos_array) + i, b);
    }
}

void radixSort(int** arr, int size)
{
    int shift = 0;
    while (shift < 32) {
        fast_sort_kernel(arr, shift,  size);
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

    posix_memalign((void**) &t1, 32, sizeof(int)*4*256);
    posix_memalign((void**) &t2, 32, sizeof(int)*4*256);
    posix_memalign((void**) &t3, 32, sizeof(int)*4*256);
    posix_memalign((void**) &t4, 32, sizeof(int)*4*256);

    for (int i = 1; i <= runs; ++i) {
        st = rdtsc();
        radixSort(&arr, size);
        et = rdtsc();

        sum += (et - st);
    }
    
    free(t1);
    free(t2);
    free(t3);
    free(t4);

    printArray(f, arr, size);
    printf("Execution time: %lf [cycles]\n", (double) (sum / (1.0 * runs)));
    free(arr);
    return (0);
}
