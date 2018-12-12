compile:
	gcc -std=c99 -O3 -o baseline.x baseline.c
	gcc -std=c99 -O3 -mavx -mfma -o optimized.x optimized.c
	gcc -std=c99 -O3 -mavx -mfma -o parallel.x parallel.c -fopenmp

run_single_core:
	perl single_core.pl

run_parallel:
	perl parallel_cores.pl

cleanup:
	rm *.x	
