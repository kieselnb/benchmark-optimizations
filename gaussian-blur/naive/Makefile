# compiles the naive gaussian blur implementation

CUDACC=nvcc

EXE=blur

CXXFLAGS=-Wall -O2 -march=native

CUDAFLAGS=-lGL -lglut

.PHONY: all
all: $(EXE)

blur: blur.cpp

blur-cuda: kernel.cu
	$(CUDACC) -o $@ $< $(CUDAFLAGS)

.PHONY: clean
clean:
	rm -f $(EXE) *.o
