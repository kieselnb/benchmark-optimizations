# compiles the naive gaussian blur implementation

EXE=blur-naive

CFLAGS=-Wall -O2 -march=native

all: $(EXE)

$(EXE): blur-naive.cpp

.PHONY: clean
clean:
	rm -f $(EXE) *.o
