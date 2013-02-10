.PHONY: run, clean

CC=gcc
CFLAGS=-Wall -fopenmp -O -ansi -pedantic

default: run

%.out: %.c
	$(CC) $(CFLAGS) -o $*.out $*.c

run: palindromic.out
	./$^

clean:
	rm *.out
