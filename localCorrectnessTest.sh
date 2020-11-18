#!/bin/bash

# Try with -Wall
gcc parallel.c -lpthread
./a.out 1000

gcc sequential.c
./a.out 1000

if cmp -s "resultParallel-16-18.txt" "resultSequential-18.txt"; then
    echo "Correct"
else
    echo "Incorrect"
fi
