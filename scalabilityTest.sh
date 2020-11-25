#!/bin/bash

gcc parallel.c -lrt -lpthread -o parallel.out
gcc sequential.c -lrt -o sequential.out

for arraySize in 277
#for arraySize in 10 100 200 400 600 800 900 1000
#for arraySize in 10 50 100 200 300 400 500 600 700 800 900 950 1000
#for arraySize in 200 400 600 800
#for arraySize in 200 600 1000 1400 1800 2200 2300 2400 2500 2600 2700
do
    for threadNum in 2 4 6 8 10 12 13 14 15 16 17 18 19 20 22 24 26 28 30 32
    #for threadNum in  1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
    #for threadNum in 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32
    #for threadNum in 1
    do
        #for priority in 1.0 0.1 0.01 0.001 0.00001 0.0000001
        #for priority in 1.0 0.1 0.01 0.001 0.0001 0.00001 0.000001
        for priority in 0.01
        do

            echo "#!/bin/bash" >> singleScaleTest-$arraySize-$priority-$threadNum.slurm
            echo "#SBATCH --account=cm30225" >> singleScaleTest-$arraySize-$priority-$threadNum.slurm
            echo "#SBATCH --partition=teaching" >> singleScaleTest-$arraySize-$priority-$threadNum.slurm
            echo "#SBATCH --job-name=Parallel-$arraySize-$priority-$threadNum" >> singleScaleTest-$arraySize-$priority-$threadNum.slurm
            echo "#SBATCH --time=00:15:00" >> singleScaleTest-$arraySize-$priority-$threadNum.slurm
            echo "#SBATCH --output=parallel-results-$arraySize-$priority-$threadNum.txt" >> singleScaleTest-$arraySize-$priority-$threadNum.slurm
            echo "#SBATCH --nodes=1" >> singleScaleTest-$arraySize-$priority-$threadNum.slurm
            echo "perf stat ./parallel.out $arraySize $priority $threadNum" >> singleScaleTest-$arraySize-$priority-$threadNum.slurm
            sbatch singleScaleTest-$arraySize-$priority-$threadNum.slurm

            echo "#!/bin/bash" >> singleScaleTest-$arraySize-$priority.slurm
            echo "#SBATCH --account=cm30225" >> singleScaleTest-$arraySize-$priority.slurm
            echo "#SBATCH --partition=teaching" >> singleScaleTest-$arraySize-$priority.slurm
            echo "#SBATCH --job-name=Sequential-$arraySize-$priority" >> singleScaleTest-$arraySize-$priority.slurm
            echo "#SBATCH --time=00:15:00" >> singleScaleTest-$arraySize-$priority.slurm
            echo "#SBATCH --output=sequential-results-$arraySize-$priority.txt" >> singleScaleTest-$arraySize-$priority.slurm
            echo "#SBATCH --nodes=1" >> singleScaleTest-$arraySize-$priority.slurm
            echo "perf stat ./sequential.out $arraySize $priority" >> singleScaleTest-$arraySize-$priority.slurm
            sbatch singleScaleTest-$arraySize-$priority.slurm

        done
    done
done

echo "#!/bin/bash" >> singleScaleTest.slurm
echo "#SBATCH --account=cm30225" >> singleScaleTest.slurm
echo "#SBATCH --partition=teaching" >> singleScaleTest.slurm
echo "#SBATCH --job-name=Finalise" >> singleScaleTest.slurm
echo "#SBATCH --nodes=1" >> singleScaleTest.slurm
echo "#SBATCH --mail-type=END" >> singleScaleTest.slurm
echo "#SBATCH --mail-user=mcb68@bath.ac.uk" >> singleScaleTest.slurm
echo "echo finished" >> singleScaleTest.slurm
sbatch singleScaleTest.slurm