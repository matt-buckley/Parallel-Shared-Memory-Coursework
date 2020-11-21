gcc parallel.c -lpthread -o parallel.out

precision=0.01

for arraySize in 2000
do
	echo "For $arraySize x $arraySize :"
	./parallel.out $arraySize $precision 12
done
