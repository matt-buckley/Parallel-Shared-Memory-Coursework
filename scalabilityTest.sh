#!/bin/bash

gcc parallel.c -lpthread -o parallel.out
gcc sequential.c -o sequential.out

for arraySize in 10 50 100 200 300 400 500 600 700 800 900 950 1000
do
    for threadNum in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
    do
        for priority in 1.0 0.1 0.01 0.001 0.0001 0.00001 0.000001
        do

            echo "#!/bin/bash" >> singleScaleTest-$arraySize-$priority-$threadNum.slurm
            echo "#SBATCH --account=cm30225" >> singleScaleTest-$arraySize-$priority-$threadNum.slurm
            echo "#SBATCH --partition=teaching" >> singleScaleTest-$arraySize-$priority-$threadNum.slurm
            echo "#SBATCH --job-name=Parallel-$arraySize-$priority-$threadNum" >> singleScaleTest-$arraySize-$priority-$threadNum.slurm
            echo "#SBATCH --time=00:15:00" >> singleScaleTest-$arraySize-$priority-$threadNum.slurm
            echo "#SBATCH --output=parallel-results-$arraySize-$priority-$threadNum.out" >> singleScaleTest-$arraySize-$priority-$threadNum.slurm
            echo "#SBATCH --nodes=1" >> singleScaleTest-$arraySize-$priority-$threadNum.slurm
            echo "perf stat -r 4 ./parallel.out $arraySize $priority $threadNum" >> singleScaleTest-$arraySize-$priority-$threadNum.slurm
            sbatch singleScaleTest-$arraySize-$priority-$threadNum.slurm

            echo "#!/bin/bash" >> singleScaleTest-$arraySize-$priority.slurm
            echo "#SBATCH --account=cm30225" >> singleScaleTest-$arraySize-$priority.slurm
            echo "#SBATCH --partition=teaching" >> singleScaleTest-$arraySize-$priority.slurm
            echo "#SBATCH --job-name=Sequential-$arraySize-$priority" >> singleScaleTest-$arraySize-$priority.slurm
            echo "#SBATCH --time=00:15:00" >> singleScaleTest-$arraySize-$priority.slurm
            echo "#SBATCH --output=sequential-results-$arraySize-$priority-$threadNum.out" >> singleScaleTest-$arraySize-$priority.slurm
            echo "#SBATCH --nodes=1" >> singleScaleTest-$arraySize-$priority.slurm
            echo "perf stat -r 4 ./sequential.out $arraySize $priority" >> singleScaleTest-$arraySize-$priority.slurm
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