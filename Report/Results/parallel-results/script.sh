for arraySize in 2400; do
	for threadNum in 18 19 20 22; do
		for FILE in parallel-results-$arraySize-0.01-$threadNum.txt; do
			code $FILE
		done
	done
done
