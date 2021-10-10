CC = gcc
CXX = g++

CFLAGS = -g -Wall -Wextra -Wpedantic
CXXFLAGS = -std=c++17 -Wall

all: clean gen prog

prog:
	$(CC) $(CFLAGS) main.c -o prog

gen:
	$(CXX) $(CXXFLAGS) gen.cpp -o gen

clean:
	rm -f prog gen
