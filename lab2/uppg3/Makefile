.PHONY: run, clean

CC=g++
CFLAGS=-std=c++0x -fopenmp -O
ifndef FP
FP=out
endif
ifndef NT
NT=8
endif

default: run

%.out: %.cpp
	$(CC) $(CFLAGS) -o $*.out $*.cpp

run: palindromic.out
	OMP_NUM_THREADS=$(NT) ./$^ $(FP)

clean:
	rm *.out
	rm out
