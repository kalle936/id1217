.PHONY: run clean

ifndef NRW
NRW=10
endif
ifndef NRM
NRM=10
endif

CXX=g++
CXXFLAGS=-g -std=c++0x -Wall
PTHREAD=-lpthread

OFILES=monitor.o

default: run

build: $(OFILES)

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< $(PTHREAD)

unisex: $(OFILES) unisex.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@ $(PTHREAD)

run: unisex
	./unisex $(NRM) $(NRW)

clean:
	rm -f *.o
	rm -f unisex
