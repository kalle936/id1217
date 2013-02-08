.PHONY: run, clean

ifndef SIZE
SIZE=1000000
endif
ifndef NWORKERS
NWORKERS=10
endif

CC=gcc
CFLAGS=-O -Wall -fopenmp -ansi -pedantic


default: run

%.out: %.c
	$(CC) $(CLFAGS) -o $*.out $*.c

run: quicksort.out
	./$^ $(SIZE) $(NWORKERS)

clean:
	rm *.out
