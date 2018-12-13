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


int comp(const void *elem1, const void *elem2)
{
    int f = *((int*) elem1);
    int s = *((int*) elem2);
    if (f > s) return (1);
    if (f < s) return (-1);
    return (0);
}

void printArray(FILE* f, int* arr, int size)
{
    for (int i = 0; i < size; ++i) {
        fprintf(f, "%d ", arr[i]);
    }

    fprintf(f, "\n");
}

int main(int argc, char **argv)
{
    int size = atoi(argv[1]);
    int runs = atoi(argv[2]);

    int *arr;
    posix_memalign((void**) &arr, 32, sizeof(int)*size);
    srand(time(0));

    for (int i = 0; i < size; ++i) {
        arr[i] = rand();
    }

    FILE *f = fopen("standart.txt", "w");
    printArray(f, arr, size);

    unsigned long long st, et, sum = 0;

    for (int i = 1; i < runs; ++i) {
        st = rdtsc();
        qsort(arr, size, sizeof(int), comp);
        et = rdtsc();

        sum += (et - st);
    }

    printArray(f, arr, size);
    printf("Execution time: %lf [cycles]\n", (double) (sum / (1.0 * runs)));
    free(arr);
    return (0);
}
