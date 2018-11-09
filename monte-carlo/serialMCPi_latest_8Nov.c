#include <immintrin.h>
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

unsigned long long kernel(float *x, float *y, unsigned long long in_points) {
	__m256 tmp = _mm256_set_ps(1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
	
	__m256 x0 = _mm256_load_ps(x+0); 
	__m256 x1 = _mm256_load_ps(x+8); 
	x0 = _mm256_mul_ps(x0, x0);
	x1 = _mm256_mul_ps(x1, x1);
	__m256 tmp1 = _mm256_load_ps(y+0);
	__m256 tmp2 = _mm256_load_ps(y+8);
	x0 = _mm256_fmadd_ps(tmp1, tmp1, x0);
	x1 = _mm256_fmadd_ps(tmp2, tmp2, x1);
	x0 = _mm256_cmp_ps(x0, tmp, _CMP_LE_OQ);	
	x1 = _mm256_cmp_ps(x1, tmp, _CMP_LE_OQ);	
	unsigned mask1 = _mm256_movemask_ps(x0); 
	unsigned mask2 = _mm256_movemask_ps(x1); 
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


int main(int argc, char* argv[])
{
	int numOfIter = atoi(argv[1]);
	int runs = atoi(argv[2]);

	unsigned long long circle_points = 0;
	float *x, *y;
	posix_memalign((void**)&x, 64, sizeof(float)*numOfIter);	
	posix_memalign((void**)&y, 64, sizeof(float)*numOfIter);	
	unsigned long long t0 = 0;
	unsigned long long t1 = 0;
	unsigned long long sum = 0;
	float pi;
	srand(time(NULL));
	for (int i = 0; i < numOfIter; i++) {
		x[i] = (float)rand() * RAND_MAX_INVERSE;
		y[i] = (float)rand() * RAND_MAX_INVERSE;
	}
	
	for (int i = 0; i < runs; i++) {	
		t0 = rdtsc();
		circle_points = 0;
		for (int j = 0; j < numOfIter; j+=80) {
			circle_points += kernel(x+j, y+j, circle_points);
		}
		t1 = rdtsc();
		sum += (t1 - t0);
	}
	float cycles = (float)(sum) / (float)(runs);
	pi = (float)(circle_points) / (float)(numOfIter) * 4.0;
    	printf("Pi: %lf\n", pi);
	float gflops = (2.4 * numOfIter * 3) / (cycles);
	printf("GFLOPS : %lf\n", gflops);
	free(x);
	free(y);
	return 0;
}
