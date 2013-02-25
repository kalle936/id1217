.PHONY: run clean

ifndef BEES
BEES=20
endif

CXX=g++
CXXFLAGS=-g -std=c++0x -Wall
PTHREAD=-lpthread

OFILES=monitor.o

default: run

build: $(OFILES)

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< $(PTHREAD)

feedthebear: $(OFILES) feedthebear.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@ $(PTHREAD)

run: feedthebear
	./feedthebear $(BEES)

clean:
	rm -f *.o
	rm -f feedthebear
