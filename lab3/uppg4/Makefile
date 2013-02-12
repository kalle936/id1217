.PHONY: run, clean

ifndef NRM
NRM=1
endif
ifndef NRW
NRW=1
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
