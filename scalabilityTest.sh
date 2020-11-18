#!/bin/bash

gcc parallel.c -lpthread -o parallel.out
gcc sequential.c -o sequential.out

for arraySize in 100 500 1000 2000 3000 4000 5000 6000 7000
do
    for threadNum in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
    do
        #for priority in 1.0 0.1 0.01 0.001 0.0001 0.00001 0.000001
        #do
        priority=0.001

        rm singleScaleTest.slurm
        echo "#!/bin/bash" >> singleScaleTest.slurm
        echo "#SBATCH --account=cm30225" >> singleScaleTest.slurm
        echo "#SBATCH --partition=teaching" >> singleScaleTest.slurm
        echo "#SBATCH --job-name=Parallel-$arraySize-$priority-$threadNum" >> singleScaleTest.slurm
        echo "#SBATCH --time=00:15:00" >> singleScaleTest.slurm
        echo "#SBATCH --output=parallel-results-$arraySize-$priority-$threadNum.out" >> singleScaleTest.slurm
        echo "#SBATCH --nodes=1" >> singleScaleTest.slurm
        echo "perf stat -r 4 ./parallel.out $arraySize $priority $threadNum" >> singleScaleTest.slurm
        sbatch singleScaleTest.slurm

        rm singleScaleTest.slurm
        echo "#!/bin/bash" >> singleScaleTest.slurm
        echo "#SBATCH --account=cm30225" >> singleScaleTest.slurm
        echo "#SBATCH --partition=teaching" >> singleScaleTest.slurm
        echo "#SBATCH --job-name=Sequential-$arraySize-$priority" >> singleScaleTest.slurm
        echo "#SBATCH --time=00:15:00" >> singleScaleTest.slurm
        echo "#SBATCH --output=sequential-results-$arraySize-$priority-$threadNum.out" >> singleScaleTest.slurm
        echo "#SBATCH --nodes=1" >> singleScaleTest.slurm
        echo "perf stat -r 4 ./sequential.out $arraySize $priority" >> singleScaleTest.slurm
        sbatch singleScaleTest.slurm

        #done
    done
done

rm singleScaleTest.slurm
echo "#!/bin/bash" >> singleScaleTest.slurm
echo "#SBATCH --account=cm30225" >> singleScaleTest.slurm
echo "#SBATCH --partition=teaching" >> singleScaleTest.slurm
echo "#SBATCH --job-name=Finalise" >> singleScaleTest.slurm
echo "#SBATCH --nodes=1" >> singleScaleTest.slurm
echo "#SBATCH --mail-type=END" >> singleScaleTest.slurm
echo "#SBATCH --mail-user=mcb68@bath.ac.uk" >> singleScaleTest.slurm
echo "echo finished" >> singleScaleTest.slurm
sbatch singleScaleTest.slurm
rm singleScaleTest.slurm