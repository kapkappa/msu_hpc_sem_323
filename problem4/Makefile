#BUILD ?= Debug
BUILD = Release

CC = gcc
CXX = g++
MPIC = mpicc

CCFLAGS = -g
CXXFLAGS = -g -std=c++11


ifeq ($(BUILD), Release)
CXXFLAGS += -O3 -ffast-math -ftree-vectorize -funroll-loops
CCFLAGS += -O3 -ffast-math -ftree-vectorize -funroll-loops
endif

ifeq ($(BUILD), Debug)
CXXFLAGS += -Wall -O0
CCFLAGS += -Wall -O0 -DDEBUG
endif

all: clean prog gen format

prog:
	$(MPIC) $(CFLAGS) main.c -o prog

gen:
	$(CXX) $(CXXFLAGS) gen.cpp -o gen

format:
	clang-format -i -style=LLVM *.c

test:
	./test.sh

clean:
	rm -rf prog gen
