#!/bin/bash
# Account & partition (must have these)
#SBATCH --account=cm30225
#SBATCH --partition=teaching
# Name of job (optional)
#SBATCH --job-name=Run_Test
#SBATCH --output=results.out
#SBATCH --time=00:15:00
# One node
#SBATCH --nodes=1
# Any normal shell stuff

arraySize=100

# Try with -Wall
gcc parallel.c -lpthread -o parallel.out
perf stat ./parallel.out $arraySize

gcc sequential.c -o sequential.out
perf stat ./sequential.out $arraySize

if cmp -s "resultParallel-16-$arraySize.txt" "resultSequential-$arraySize.txt"; then
    echo "Correct"
else
    echo "Incorrect"
fi