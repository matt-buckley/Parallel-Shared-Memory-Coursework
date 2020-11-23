gcc parallel.c -lpthread -o parallel.out

precision=0.01

#for arraySize in 200 600 1000 1400 1800 2200 2300 2400 2500 2600 2700
for arraySize in 2300
do
	echo "For $arraySize x $arraySize :"
	./parallel.out $arraySize $precision 12
done
