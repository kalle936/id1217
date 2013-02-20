.PHONY: run clean

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
	./unisex

clean:
	rm -f *.o
