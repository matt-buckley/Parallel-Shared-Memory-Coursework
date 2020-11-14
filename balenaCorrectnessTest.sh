#!/bin/bash
# Account & partition (must have these)
#SBATCH --account=cm30225
#SBATCH --partition=teaching
# Name of job (optional)
#SBATCH --job-name=Run_Test
#SBATCH --output=results.out
# One node
#SBATCH --nodes=1
# Any normal shell stuff

# Try with -Wall
gcc parallel.c -lpthread -o parallel.out
perf stat ./parallel.out 2000

gcc sequential.c -o sequential.out
perf stat ./sequential.out 2000

if cmp -s "resultParallel.txt" "resultSequential.txt"; then
    echo "Correct"
else
    echo "Incorrect"
fi