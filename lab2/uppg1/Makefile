.PHONY: clean run

CC = gcc
CFLAGS = -O -fopenmp -Wall -ansi -pedantic
ifndef SIZE
SIZE = 10000
endif
ifndef NWORKERS
NWORKERS = 10
endif

default: run

%.out: %.c
	$(CC) $(CFLAGS) -o $*.out $*.c

run: matrixSum-openmp.out
	./$^ $(SIZE) $(NWORKERS)

clean:
	rm *.out
