.PHONY: clean, run

CC=gcc
CFLAGS=-g -Wall -ansi -pedantic
PTHREAD=-lpthread

ifndef BEES
BEES=20
endif

default: run

run: feedthebear.out
	./$< $(BEES)

%.out: %.c
	$(CC) $(CFLAGS) -o $@ $< $(PTHREAD)

clean:
	rm *.out
