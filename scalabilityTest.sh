#!/bin/bash

gcc parallel.c -lpthread -o parallel.out
gcc sequential.c -o sequential.out

#for arraySize in 10 100 200 400 600 800 900 1000
for arraySize in 10 50 100 200 300 400 500 600 700 800 900 950 1000
do
    #for threadNum in 2 4 6 8 10 12 14 16
    for threadNum in  1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
    do
        #for priority in 1.0 0.1 0.01 0.001 0.00001 0.0000001
        for priority in 1.0 0.1 0.01 0.001 0.0001 0.00001 0.000001
        do

            echo "#!/bin/bash" >> fullTest-singleScaleTest-$arraySize-$priority-$threadNum.slurm
            echo "#SBATCH --account=cm30225" >> fullTest-singleScaleTest-$arraySize-$priority-$threadNum.slurm
            echo "#SBATCH --partition=teaching" >> fullTest-singleScaleTest-$arraySize-$priority-$threadNum.slurm
            echo "#SBATCH --job-name=Parallel-$arraySize-$priority-$threadNum" >> fullTest-singleScaleTest-$arraySize-$priority-$threadNum.slurm
            echo "#SBATCH --time=00:15:00" >> fullTest-singleScaleTest-$arraySize-$priority-$threadNum.slurm
            echo "#SBATCH --output=fullTest-parallel-results-$arraySize-$priority-$threadNum.out" >> fullTest-singleScaleTest-$arraySize-$priority-$threadNum.slurm
            echo "#SBATCH --nodes=1" >> fullTest-singleScaleTest-$arraySize-$priority-$threadNum.slurm
            echo "perf stat -r 4 ./parallel.out $arraySize $priority $threadNum" >> fullTest-singleScaleTest-$arraySize-$priority-$threadNum.slurm
            sbatch fullTest-singleScaleTest-$arraySize-$priority-$threadNum.slurm

            echo "#!/bin/bash" >> fullTest-singleScaleTest-$arraySize-$priority.slurm
            echo "#SBATCH --account=cm30225" >> fullTest-singleScaleTest-$arraySize-$priority.slurm
            echo "#SBATCH --partition=teaching" >> fullTest-singleScaleTest-$arraySize-$priority.slurm
            echo "#SBATCH --job-name=Sequential-$arraySize-$priority" >> fullTest-singleScaleTest-$arraySize-$priority.slurm
            echo "#SBATCH --time=00:15:00" >> fullTest-singleScaleTest-$arraySize-$priority.slurm
            echo "#SBATCH --output=fullTest-sequential-results-$arraySize-$priority-$threadNum.out" >> fullTest-singleScaleTest-$arraySize-$priority.slurm
            echo "#SBATCH --nodes=1" >> fullTest-singleScaleTest-$arraySize-$priority.slurm
            echo "perf stat -r 4 ./sequential.out $arraySize $priority" >> fullTest-singleScaleTest-$arraySize-$priority.slurm
            sbatch fullTest-singleScaleTest-$arraySize-$priority.slurm

        done
    done
done

echo "#!/bin/bash" >> fullTest-singleScaleTest.slurm
echo "#SBATCH --account=cm30225" >> fullTest-singleScaleTest.slurm
echo "#SBATCH --partition=teaching" >> fullTest-singleScaleTest.slurm
echo "#SBATCH --job-name=Finalise" >> fullTest-singleScaleTest.slurm
echo "#SBATCH --nodes=1" >> fullTest-singleScaleTest.slurm
echo "#SBATCH --mail-type=END" >> fullTest-singleScaleTest.slurm
echo "#SBATCH --mail-user=mcb68@bath.ac.uk" >> fullTest-singleScaleTest.slurm
echo "echo finished" >> fullTest-singleScaleTest.slurm
sbatch fullTest-singleScaleTest.slurm