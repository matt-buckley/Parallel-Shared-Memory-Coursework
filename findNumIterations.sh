gcc parallel.c -lrt -lpthread -o parallel.out

precision=0.01
#for arraySize in 550
for arraySize in 50 100 150 200 250 300 350 400 450 500 550
do
	echo "For $arraySize x $arraySize :"
	./parallel.out $arraySize $precision 4
done
