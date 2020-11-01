#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// This is always going to be 4, and passing more arguments to make it reusable would increase unnecessary communication
// Should this even be in a function?
double meanOfFour(double *nums) {
    double sum = 0;

    int i;
    for (i = 0; i < 4; i++) {
        sum += nums[i];
    }

    return sum / 4;
}

int main(int argc, char *argv[]) {

    // Any way to make these const?
    int arraySize;
    double precision;

    // Define dimension
    if (argc > 1) {
        arraySize = atoi(argv[1]);
    } else {
        arraySize = 5; //default
    }

    // Define precision
    if (argc > 2) {
        char *temp;
        precision = strtod(argv[2], &temp);
    } else {
        precision = 0.001;
    }



    // Change to use malloc
    //double testArray[arraySize][arraySize];

    // HARDCODED
    double testArray[5][5] = {
        {1.0, 1.0, 1.0, 1.0, 1.0},
        {1.0, 0.0, 0.0, 0.0, 1.0},
        {1.0, 0.0, 0.0, 0.0, 1.0},
        {1.0, 0.0, 0.0, 0.0, 1.0},
        {1.0, 1.0, 1.0, 1.0, 1.0}
    };

    //double testArray[4][4] = {
    //    {1.0, 1.0, 1.0, 1.0},
    //    {1.0, 0.0, 0.0, 1.0},
    //    {1.0, 0.0, 0.0, 1.0},
    //    {1.0, 1.0, 1.0, 1.0}
    //};

    // A lot of this needs changing, including nested for loops, assigning values unnecessarily, and more
    double calcPrecision = 1.0;
    int iterationNum = 0;

    // Here as does all calculations on 'old' values before updating, rather than
    double newArray[5][5];
    memcpy(newArray, testArray, arraySize * arraySize * sizeof(double));
    int i, j;
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 5; j++) {
            printf("%f\t", newArray[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    
    do {
        double biggestDiff = 0.0;
        for (i = 0; i < arraySize; i++) {
            for (j = 0; j < arraySize; j++) {
                if (i != 0 && j != 0 && i != arraySize - 1 && j != arraySize - 1) {
                    double nums[4] = {testArray[i-1][j], testArray[i][j-1], testArray[i+1][j], testArray[i][j+1]};
                    double avg = meanOfFour(nums);
                    double diff = fabs(testArray[i][j] - avg);
                    newArray[i][j] = avg;
                    if (diff > biggestDiff) {
                        biggestDiff = diff;
                        calcPrecision = diff;
                    }
                }
            }
        }
        iterationNum += 1;
        memcpy(testArray, newArray, arraySize * arraySize * sizeof(double));

        for (i = 0; i < 5; i++) {
            for (j = 0; j < 5; j++) {
                printf("%f\t", newArray[i][j]);
            }
            printf("\n");
        }
        printf("\n");

    } while (calcPrecision > precision); //comparison here as will already do at least once

    printf("After %d iterations:\n\n", iterationNum);
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 5; j++) {
            printf("%f\t", testArray[i][j]);
        }
        printf("\n");
    }

    return 0;

}