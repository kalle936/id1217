.PHONY: run, clean

ifndef NRM
NRM=10
endif
ifndef NRW
NRW=10
endif

CC=gcc
CFLAGS=-g -Wall -ansi -pedantic
PTHREAD=-lpthread

%.out: %.c
	$(CC) $(CFLAGS) -o $@ $< $(PTHREAD)

run: unisex.out
	./$< $(NRM) $(NRW)

clean:
	rm *.out
