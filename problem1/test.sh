#!/bin/bash

for i in 0 1 2 3 4 5; do
    ./prog A32 B32 C32 $i;
done;

for i in 0 1 2 3 4 5; do
    ./prog A64 B64 C64 $i;
done;
