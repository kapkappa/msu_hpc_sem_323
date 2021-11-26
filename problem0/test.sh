#!/bin/bash

echo "10^8 segments, with 1 threads"
./prog 100000000 1

echo "10^8 segments, with 2 threads"
./prog 100000000 2

echo "10^8 segments, with 4 threads"
./prog 100000000 4

echo "10^8 segments, with 6 threads"
./prog 100000000 6

echo "10^8 segments, with 8 threads"
./prog 100000000 8

echo "10^8 segments, with 12 threads"
./prog 100000000 12

echo "10^9 segments, with 1 threads"
./prog 1000000000 1

echo "10^9 segments, with 2 threads"
./prog 1000000000 2

echo "10^9 segments, with 4 threads"
./prog 1000000000 4

echo "10^9 segments, with 6 threads"
./prog 1000000000 6

echo "10^9 segments, with 8 threads"
./prog 1000000000 8

echo "10^9 segments, with 12 threads"
./prog 1000000000 12

echo "10^10 segments, with 1 threads"
./prog 10000000000 1

echo "10^10 segments, with 2 threads"
./prog 10000000000 2

echo "10^10 segments, with 4 threads"
./prog 10000000000 4

echo "10^10 segments, with 6 threads"
./prog 10000000000 6

echo "10^10 segments, with 8 threads"
./prog 10000000000 8

echo "10^10 segments, with 12 threads"
./prog 10000000000 12
