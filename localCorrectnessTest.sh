#!/bin/bash

# Try with -Wall
gcc parallel.c -lpthread -lrt
./a.out 100 0.01 8

gcc sequential.c -lrt
./a.out 100 0.01

if cmp -s "resultParallel-8-100.txt" "resultSequential-100.txt"; then
    echo "Correct"
else
    echo "Incorrect"
fi
