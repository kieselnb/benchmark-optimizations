#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#define RAND_MAX_INVERSE 0.00000000046566128752

//timing routine for reading the time stamp counter
static __inline__ unsigned long long rdtsc(void) {
	unsigned hi, lo;
	__asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
	return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}

unsigned long long calculatePI(int num_of_iterations, float *x, float *y) {
	unsigned long long circle_points = 0;
	float z;
	for (int i=0; i<num_of_iterations; i++) {
		z = x[i]*x[i] + y[i]*y[i];
		if (z <= 1) {
			++circle_points;
		}
	}
	return circle_points;
}

int main(int argc, char* argv[])
{
	int numOfIter = atoi(argv[1]);
	int runs = atoi(argv[2]);
	
	float *x, *y;
        posix_memalign((void**)&x, 64, sizeof(float)*numOfIter);
        posix_memalign((void**)&y, 64, sizeof(float)*numOfIter);
	
	unsigned long long circle_points = 0;
	unsigned long long t0 = 0;
	unsigned long long t1 = 0;
	unsigned long long sum = 0;
	float pi;
	srand(time(NULL));
	
	for (int i = 0; i <= numOfIter; i++) {
		x[i] = (float)rand() * RAND_MAX_INVERSE;
                y[i] = (float)rand() * RAND_MAX_INVERSE;
	}
	
	for (int i = 0; i < runs; i++) {	
		t0 = rdtsc();
		circle_points = calculatePI(numOfIter, x, y);
		t1 = rdtsc();
		sum += (t1 - t0);
	}
	float cycles = (float)(sum) / (float)(runs);
	pi = (float)(circle_points) / (float)(numOfIter) * 4.0;
    	printf("Pi: %lf\n", pi);
	float gflops = 2.4 * numOfIter * 2 / cycles;
	printf("GFLOPS : %lf\n", gflops);
	free(x);
	free(y);
	return 0;
}
