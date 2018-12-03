# builds the optimized version of Gaussian blur

EXES=blur blur-shuffle

CFLAGS=-Wall -march=native -O2

# stb image needs math library (pow)
LDFLAGS=-lm

.PHONY: all
all: blur blur-shuffle

blur: blur.c

blur-shuffle: blur-shuffle.c

clean:
	rm -f blur blur-shuffle *.o
