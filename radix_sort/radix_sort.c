// C implementation of Radix Sort         
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
unsigned long long rdtsc()
{
    unsigned long long int x;
    unsigned a, d;
    __asm__ volatile("rdtsc" : "=a" (a), "=d" (d));
    return ((unsigned long long) a) | (((unsigned long long) d) << 32);
}

// A utility function to get maximum value in arr[]
int getMax(int* arr, int n)
{
    int mx = arr[0];
    int i;
    for (i = 1; i < n; i++)
        if (arr[i] > mx)
            mx = arr[i];
    return mx;
}

// A function to do counting sort of arr[] according to
// the digit represented by exp.
void countSort(int* arr, int n, int exp)
{
    int* output; // output array
    posix_memalign((void**) &output, 32, sizeof(int)*n);

    int i, count[10] = {0};

    // Store count of occurrences in count[]
    for (i = 0; i < n; i++)
        count[ (arr[i]/exp)%10 ]++;
    // Change count[i] so that count[i] now contains actual
    //  position of this digit in output[]
    for (i = 1; i < 10; i++)
        count[i] += count[i - 1];
    // Build the output array
    for (i = n - 1; i >= 0; i--)
    {
        output[count[ (arr[i]/exp)%10 ] - 1] = arr[i];
        count[ (arr[i]/exp)%10 ]--;
    }

    // Copy the output array to arr[], so that arr[] now
    // contains sorted numbers according to current digit
    for (i = 0; i < n; i++)
        arr[i] = output[i];
    free(output);
}

// The main function to that sorts arr[] of size n using
// Radix Sort
void radixsort(int* arr, int n)
{
    // Find the maximum number to know number of digits
    int m = getMax(arr, n);

    // Do counting sort for every digit. Note that instead
    // of passing digit number, exp is passed. exp is 10^i
    // where i is current digit number
    int exp;
    for (exp = 1; m/exp > 0; exp *= 10)
        countSort(arr, n, exp);
}

// A utility function to print an array
void printArray(FILE* f, int* arr, int size)
{
    for (int i = 0; i < size; ++i) {
        fprintf(f, "%d ", arr[i]);
    }

    fprintf(f, "\n");
}

// Driver program to test above functions
int main(int argc, char** argv)
{
    int inputSize = atoi(argv[1]);
    int runs = atoi(argv[2]);

    int* arr;
    posix_memalign((void**) &arr, 32, sizeof(int)*inputSize);

    srand(time(0));
    for (int i = 0; i < inputSize; ++i) {
        arr[i] = rand();
    }

    FILE* f = fopen("result_orig.txt", "w");
    printArray(f, arr, inputSize);
    unsigned long long st, et, sum = 0;

    for (int i = 0; i < runs; ++i) {
        st = rdtsc();
        radixsort(arr, inputSize);
        et = rdtsc();

        sum += (et - st);
    }

    printArray(f, arr, inputSize);
    printf("Execution time: %lf [cycles]\n", (double) (sum / (1.0 * runs)));

    free(arr);
    return 0;
}
