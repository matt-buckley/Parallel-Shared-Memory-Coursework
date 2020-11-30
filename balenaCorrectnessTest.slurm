#!/bin/bash
# Account & partition (must have these)
#SBATCH --account=cm30225
#SBATCH --partition=teaching
# Name of job (optional)
#SBATCH --job-name=Correctness-Test
#SBATCH --output=correctness.out
#SBATCH --time=00:15:00
# Notification
#SBATCH --mail-type=END
#SBATCH --mail-user=mcb68@bath.ac.uk
# One node
#SBATCH --nodes=1
# Any normal shell stuff

# Will timeout but just let it keep running till then and restart

precision=0.01
numThreads=15

for arraySize in 50 100 150 200 250 300
#for arraySize in 350 400 450 500 550
do

    gcc parallel.c -lpthread -lrt -o parallel.out
    ./parallel.out $arraySize $precision $numThreads

    gcc sequential.c -lrt -o sequential.out
    ./sequential.out $arraySize $precision

    if cmp -s "resultParallel-$numThreads-$arraySize.txt" "resultSequential-$arraySize.txt"; then
        echo "$arraySize: Correct"
    else
        echo "$arraySize: Incorrect"
    fi

done