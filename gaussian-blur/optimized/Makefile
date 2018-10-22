# builds the optimized version of Gaussian blur

EXE=blur

CFLAGS=-Wall -O2 -march=native

# stb image needs math library (pow)
LDFLAGS=-lm

.PHONY: all
all: $(EXE)

$(EXE): blur.c

clean:
	rm -f $(EXE) *.o
