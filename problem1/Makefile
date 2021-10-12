BUILD ?= Debug
#BUILD = Release

CC = gcc
CXX = g++

CFLAGS = -O0 -Wall -Werror -Wextra -Wpedantic
CXXFLAGS = -std=c++17 -Wall

ifeq ($(BUILD), Debug)
CFLAGS += -fsanitize=address
endif

all: clean gen prog

prog:
	$(CC) $(CFLAGS) main.c -o prog

gen:
	$(CXX) $(CXXFLAGS) gen.cpp -o gen

clean:
	rm -f prog gen
