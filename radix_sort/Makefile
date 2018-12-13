fastest:
	g++ -o final_radix_sort_kernel.x final_radix_sort_kernel.c -mavx -mavx2 -O3
	./final_radix_sort_kernel.x 256 10000
fast:
	g++ -o less_iter_radix.x less_iter_radix.c -O3
	./unrolled_less_iter_radix.x 256 10000
slow:
	g++ -o radix_sort.x radix_sort.c -O3
	./radix_sort.x 256 10000
library_sort:
	g++ -o standard_sort.x standard_sort.c -O3
	./standard_sort.x 256 10000

clean:
	rm -rf *.x
	
