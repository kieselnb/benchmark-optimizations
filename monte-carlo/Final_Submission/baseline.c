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

static int round_up(int size, int n)
{
    return (n * ((size + (n-1)) / n));
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
	int num_of_mc = atoi(argv[1]);
	int runs = atoi(argv[2]);
	
	//Rounding up to multiples of 80 to match the optimized kernel accuracy
	int numOfIter = round_up(num_of_mc, 80);
	
	//Allocating space to pre-generate numOfIter random (x,y) between (0,0) and (1,1)
	float *x, *y;
        posix_memalign((void**)&x, 64, sizeof(float)*numOfIter);
        posix_memalign((void**)&y, 64, sizeof(float)*numOfIter);
	
	unsigned long long circle_points = 0;
	unsigned long long t0 = 0;
	unsigned long long t1 = 0;
	unsigned long long sum = 0;
	double pi = 0.0;
	srand(time(NULL));
	
	//Pre-generating and storing random (x,y)
	for (int i = 0; i <= numOfIter; i++) {
		x[i] = (float)rand() * RAND_MAX_INVERSE;
                y[i] = (float)rand() * RAND_MAX_INVERSE;
	}
	
	//calculatePI is called here runs number of times
	for (int i = 0; i < runs; i++) {	
		t0 = rdtsc();
		circle_points = calculatePI(numOfIter, x, y);
		t1 = rdtsc();
		sum += (t1 - t0);
	}
	float cycles = (float)(sum)/(float)(runs);
	pi = 4.0 * (double)(circle_points)/(double)(numOfIter);
    	printf("Pi: %lf\n", pi);
	//Empirical peak = (Base Freq x Num of FLOPs)/Recorded number of cycles 
	float gflops = 2.4 * numOfIter * 3 / cycles;
	printf("GFLOPS : %f\n", gflops);
	free(x);
	free(y);
	return 0;
}
