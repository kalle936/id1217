.PHONY: run, clean

ifndef NRM
NRM=4
endif
ifndef NRW
NRW=4
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
