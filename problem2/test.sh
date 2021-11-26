#!/bin/bash

for i in 0 1 2 3 4 5; do
    ./prog_L1_load A32 B32 C32 $i
done

for i in 0 1 2 3 4 5; do
    ./prog_L1_store A32 B32 C32 $i
done

for i in 0 1 2 3 4 5; do
    ./prog_L1_cache A32 B32 C32 $i
done

for i in 0 1 2 3 4 5; do
    ./prog_L2_load A32 B32 C32 $i
done

for i in 0 1 2 3 4 5; do
    ./prog_L2_store A32 B32 C32 $i
done

for i in 0 1 2 3 4 5; do
    ./prog_L2_cache A32 B32 C32 $i
done
