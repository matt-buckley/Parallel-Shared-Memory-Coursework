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
perf stat ./parallel.out 7000

gcc sequential.c -o sequential.out
perf stat ./sequential.out 7000

if cmp -s "resultParallel-16-7000.txt" "resultSequential-7000.txt"; then
    echo "Correct"
else
    echo "Incorrect"
fi