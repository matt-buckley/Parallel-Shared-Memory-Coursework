#!/bin/bash

# Try with -Wall
gcc parallel.c -lpthread
./a.out

gcc sequential.c
./a.out

if cmp -s "resultParallel-16-18.txt" "resultSequential-18.txt"; then
    echo "Correct"
else
    echo "Incorrect"
fi