.PHONY: clean
CC=gcc
CCFLAGS=-g -Wall -ansi -pedantic -O
PTHREAD=-lpthread

default: matrixSum11.out matrixSum12.out matrixSum13.out

%.out: %.c
	$(CC) $(CCFLAGS) -o $*.out $*.c $(PTHREAD)

clean:
	rm *.out
