#!/bin/bash

rm -f *64

size1="1000"
size2="1"
matrix_name="A64"
vector_name="b64"
type="l"

./gen <<EOF
$size1
$size1
$type
$matrix_name
EOF

./gen <<EOF
$size1
$size2
$type
$vector_name
EOF

for i in `seq 1 4`; do
    mpirun -n $i ./prog A64 b64 y64
done;
