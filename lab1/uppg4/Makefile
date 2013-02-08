.PHONY : clean

CC=gcc
CCFLAGS=-g -Wall
PTHREAD=-lpthread

default: tee.out

tee.out: tee.c
	$(CC) $(CCFLAGS) -o tee.out tee.c $(PTHREAD)

clean:
	rm *.out
