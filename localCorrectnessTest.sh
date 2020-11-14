#!/bin/bash

# Try with -Wall
gcc parallel.c -lpthread
./a.out

gcc sequential.c
./a.out

if cmp -s "resultParallel.txt" "resultSequential.txt"; then
    echo "Correct"
else
    echo "Incorrect"
fi