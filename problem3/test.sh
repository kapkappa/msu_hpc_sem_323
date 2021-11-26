#!/bin/bash

./prog_qs 1000000
./prog_omp 1000000 1
./prog_omp 1000000 2
./prog_omp 1000000 4
./prog_omp 1000000 6
./prog_omp 1000000 8
./prog_omp 1000000 12
