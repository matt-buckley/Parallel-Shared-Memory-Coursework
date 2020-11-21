gcc parallel.c -lpthread -o parallel.out

precision=0.001

for arraySize in 100 200 400 600 800 900 1000
do
	echo "For $arraySize x $arraySize :"
	./parallel.out $arraySize $precision 15
done
