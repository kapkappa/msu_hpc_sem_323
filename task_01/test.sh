#!/bin/bash

echo "Check with adress sanitizer"
./test 10000000000 6

echo "10 billion segments, with 1 threads"
./prog 10000000000 1

echo "10 billion segments, with 2 threads"
./prog 10000000000 2

echo "10 billion segments, with 4 threads"
./prog 10000000000 4
