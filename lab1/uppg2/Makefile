CC=gcc
CCFLAGS=-g -Wall
PTHREAD=-lpthread

default: quicksort.out

quicksort.out: quicksort.c
	$(CC) $(CCFLAGS) -o quicksort.out quicksort.c $(PTHREAD)

clean:
	rm *.out
