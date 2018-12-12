#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>
//RAND_MAX_INVERSE is precalculated value of 1 / RAND_MAX
//This was done to avoid division operation in rand number generation between 0 and 1
#define RAND_MAX_INVERSE 0.00000000046566128752

//timing routine for reading the time stamp counter
//Provided by 18645 staff
static __inline__ unsigned long long rdtsc(void) {
	unsigned hi, lo;
	__asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
	return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}

//Function Description : Rounds size up to next multiple of n
static int round_up(int size, int n)
{
    return (n * ((size + (n-1)) / n));
}

//Function Description : kernel for determining #of points inside circle
//@args
//$argv1 - pointer to the start of 80 elements in array x of random floats
//$argv2 - pointer to the start of 80 elements in array y of random floats
//$argv3 - variable storing number of points inside circle - which gets updated
//everytime the kernel is called.
//Function return - function returns the updated value of 'in_points' i.e.
//points inside the circle
unsigned long long kernel(float *x, float *y, unsigned long long in_points) {
	//Tmp register storing all ones - will be referenced later for compare
	//operations of x2 + y2 <= 1
	__m256 tmp = _mm256_set_ps(1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
	
	//loads 8 x random floats to reg x0 & x1 resp.
	__m256 x0 = _mm256_load_ps(x+0); 
	__m256 x1 = _mm256_load_ps(x+8); 
	//calculates x0^2 and stores in x0
	x0 = _mm256_mul_ps(x0, x0);
	//calculates x1^2 and stores in x1
	x1 = _mm256_mul_ps(x1, x1);
	//Since, calculation of x^2 + y^2 actually
	//depends on x^2 - I am using just 2 regs for 
	//loading y - bcoz next operation which uses FMA
	//is not independent
	//Loading 8 random y float values in tmp1 and tmp2 regs resp.
	__m256 tmp1 = _mm256_load_ps(y+0);
	__m256 tmp2 = _mm256_load_ps(y+8);
	//calcuting x^2+y^2 and storing back in x0 and x1 resp.
	x0 = _mm256_fmadd_ps(tmp1, tmp1, x0);
	x1 = _mm256_fmadd_ps(tmp2, tmp2, x1);
	//comparing x^2 + y^2 with 1 (tmp reg at top)
	//storing back in x0 and x1 resp.
	x0 = _mm256_cmp_ps(x0, tmp, _CMP_LE_OQ);	
	x1 = _mm256_cmp_ps(x1, tmp, _CMP_LE_OQ);
	//using movemask to make all non-zero nan(0xFFFF) -> 1	
	unsigned mask1 = _mm256_movemask_ps(x0); 
	unsigned mask2 = _mm256_movemask_ps(x1); 
	//finding the number of 1s in each mask1 and mask2
	//Updating count1 and count2 resp.
	int count1 = _mm_popcnt_u32(mask1); 
	int count2 = _mm_popcnt_u32(mask2); 
	
	__m256 x2 = _mm256_load_ps(x+16); 
	__m256 x3 = _mm256_load_ps(x+24); 
	x2 = _mm256_mul_ps(x2, x2);
	x3 = _mm256_mul_ps(x3, x3);
	tmp1 = _mm256_load_ps(y+16);
	tmp2 = _mm256_load_ps(y+24);
	x2 = _mm256_fmadd_ps(tmp1, tmp1, x2);
	x3 = _mm256_fmadd_ps(tmp2, tmp2, x3);
	x2 = _mm256_cmp_ps(x2, tmp, _CMP_LE_OQ);	
	x3 = _mm256_cmp_ps(x3, tmp, _CMP_LE_OQ);	
	mask1 = _mm256_movemask_ps(x2); 
	mask2 = _mm256_movemask_ps(x3); 
	count1 = count1 + _mm_popcnt_u32(mask1);
	count2 = count2 + _mm_popcnt_u32(mask2);

	__m256 x4 = _mm256_load_ps(x+32); 
	__m256 x5 = _mm256_load_ps(x+40);
	x4 = _mm256_mul_ps(x4, x4);
	x5 = _mm256_mul_ps(x5, x5);
	tmp1 = _mm256_load_ps(y+32);
	tmp2 = _mm256_load_ps(y+40);
	x4 = _mm256_fmadd_ps(tmp1, tmp1, x4);
	x5 = _mm256_fmadd_ps(tmp2, tmp2, x5);
	x4 = _mm256_cmp_ps(x4, tmp, _CMP_LE_OQ);	
	x5 = _mm256_cmp_ps(x5, tmp, _CMP_LE_OQ);	
	mask1 = _mm256_movemask_ps(x4); 
	mask2 = _mm256_movemask_ps(x5); 
	count1 = count1 + _mm_popcnt_u32(mask1);
	count2 = count2 + _mm_popcnt_u32(mask2);

	__m256 x6 = _mm256_load_ps(x+48);
	__m256 x7 = _mm256_load_ps(x+56);
	x6 = _mm256_mul_ps(x6, x6);
	x7 = _mm256_mul_ps(x7, x7);
	tmp1 = _mm256_load_ps(y+48);
	tmp2 = _mm256_load_ps(y+56);
	x6 = _mm256_fmadd_ps(tmp1, tmp1, x6);
	x7 = _mm256_fmadd_ps(tmp2, tmp2, x7);
	x6 = _mm256_cmp_ps(x6, tmp, _CMP_LE_OQ);	
	x7 = _mm256_cmp_ps(x7, tmp, _CMP_LE_OQ);	
	mask1 = _mm256_movemask_ps(x6); 
	mask2 = _mm256_movemask_ps(x7); 
	count1 = count1 + _mm_popcnt_u32(mask1);
	count2 = count2 + _mm_popcnt_u32(mask2);
	
	__m256 x8 = _mm256_load_ps(x+64);
	__m256 x9 = _mm256_load_ps(x+72);
	x8 = _mm256_mul_ps(x8, x8);
	x9 = _mm256_mul_ps(x9, x9);
	tmp1 = _mm256_load_ps(y+64);
	tmp2 = _mm256_load_ps(y+72);
	x8 = _mm256_fmadd_ps(tmp1, tmp1, x8);
	x9 = _mm256_fmadd_ps(tmp2, tmp2, x9);
	x8 = _mm256_cmp_ps(x8, tmp, _CMP_LE_OQ);	
	x9 = _mm256_cmp_ps(x9, tmp, _CMP_LE_OQ);	
	mask1 = _mm256_movemask_ps(x8); 
	mask2 = _mm256_movemask_ps(x9); 
	count1 = count1 + _mm_popcnt_u32(mask1);
	count2 = count2 + _mm_popcnt_u32(mask2);
	
	in_points = count1 + count2;
	return in_points; 
}

//Function Description : main
//@args 
//$argv1 - number of monte carlo random points to be tested
//$argv2 - number of runs to get sane number of cycles
int main(int argc, char* argv[])
{
	// Reading the inputs from the user
	int num_of_mc = atoi(argv[1]);
	int runs = atoi(argv[2]);
	int num_of_threads = atoi(argv[3]);
	
	//Round up to multiples of 80 - because kernel can simulate 80 points
	//at a time for each thread
	int numOfIter = round_up(num_of_mc, 80*num_of_threads);
	
	//initialization 
	//circle points is for storing the number of points inside the circle
	//t0 is to store the start of cycle count
	//t1 is to store the end of cycle count 
	//sum is to record the number of cycles across all 'runs'
	unsigned long long circle_points_sum = 0;
	unsigned long long t0 = 0;
	unsigned long long t1 = 0;
	unsigned long long sum = 0;
	//allocating memory to store randomly generated values of x and y
	//There will be 'numOfIter' number of values generated 
	float *x, *y;
	posix_memalign((void**)&x, 64, sizeof(float)*numOfIter);	
	posix_memalign((void**)&y, 64, sizeof(float)*numOfIter);	

        //Size of unsigned long long is 4. Size of cacheline is 64B. To avoid
        //cache misses due to false sharing, declared 64/4=8 times are number of
        //threads memory, so that they have their own dedicated counter and no
        //unwanted cache misses.
	unsigned long long *circle_points;
        posix_memalign((void**)&circle_points, 64, sizeof(unsigned long long)*num_of_threads*8);
	
	//Variable declared & initialized to 0 
	//to store the value of 'pi' later on
	double pi = 0.0;
	
	//Initializing the seed for the random number generator
	srand(time(NULL));
	
	//Pre-generating 'numOfIter' number of x and y random float numbers
	for (int i = 0; i < numOfIter; i++) {
		x[i] = (float)rand() * RAND_MAX_INVERSE;
		y[i] = (float)rand() * RAND_MAX_INVERSE;
	}
	
	//Actual kernel for calculating value of 'pi'
	//runs inside this for loop
	//Outer for loop runs for 'runs' number of times
	//to avoid noises in calculation of number of cycles
	for (int i = 0; i < runs; i++) {	
		circle_points_sum = 0;
		//For every new run re-initialise value of 
		//number of points inside the circle
		//else it may add up across the 'runs'
   		for (int j = 0; j < num_of_threads; j++) {
			int ind = j * 8;
                	circle_points[ind] = 0;
                }		
		
		//This loop runs the kernel for numOfIter/80 times
		//Recommended to run in multiples of 80		
		t0 = rdtsc();
		#pragma omp parallel for num_threads(num_of_threads)
		for (int j = 0; j < numOfIter; j+=80) {
			int tid = omp_get_thread_num();
			int index = tid * 8;
			circle_points[index] += kernel(x+j, y+j, circle_points[index]);
		}
                for (int j = 0; j < num_of_threads; j++) {
			int ind = j * 8;
                        circle_points_sum += circle_points[ind];
                }
		t1 = rdtsc();
		sum += (t1 - t0);
	}
	//Calculate the number of cycles
	float cycles = (float)(sum) / (float)(runs);
	//Calculate the value of pi
	//This value of circle_points will actually come from the last run of 'runs'
	pi = (float)(circle_points_sum) / (float)(numOfIter) * 4.0;
    	printf("Pi: %lf\n", pi);
	//empirical value of peak
	//To calculate gflops, we would need time
	//but we have number of cycles reported at base freq = 2.4GHz
	//So actual cycles would be
	// = cycles x 3.3GHz(Turbo Mode Freq) / 2.4GHz(Base Freq)
	// Now, since for gflops we need time
	// Time = (cycles x 3.3GHz / 2.4GHz) / 3.3GHz
	// Time = cycles / 2.4GHz
	// GFlops = Total #FLOPS / Time
	// And hence what you see below.
	float gflops = (2.4 * numOfIter * 3) / (cycles);
	printf("GFLOPS : %f\n", gflops);

	//De-allocating the previously allocated memory for storing random x and y floats
	free(x);
	free(y);
	free(circle_points);
	
	return 0;
}
