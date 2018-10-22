# compiles the naive gaussian blur implementation

EXE=blur

CXXFLAGS=-Wall -O2 -march=native

.PHONY: all
all: $(EXE)

$(EXE): blur.cpp

.PHONY: clean
clean:
	rm -f $(EXE) *.o
