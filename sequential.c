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
    int arraySize = 5;
    double precision = 0.001;
    // For iteration
    int i, j;

    // Define dimension
    if (argc > 1) {
        arraySize = atoi(argv[1]);
    }

    // Define precision
    if (argc > 2) {
        char *temp;
        precision = strtod(argv[2], &temp);
        if (precision <= 0 || precision > 1) {
            printf("Error with entered precision, please enter a precision between 0 and 1\n");
            return 0;
        }
    }
    double testArray[arraySize][arraySize];

    if (argc > 3) {
        /* Omitted because requires using -lm compile flag
        if ((int) sqrt((double )argc - 4) != arraySize) {
            printf("Error - entered array does not match array size.");
            return 0;
        }
        */
        int argcCounter;
        i = -1;
        j = 0;
        char *temp;
        for (argcCounter = 3; argcCounter < argc; argcCounter++) {
            // Reset to start of next line
            if (((argcCounter - 3) % arraySize) == 0) {
                i += 1;
                j = 0;
            }
            else {
                j += 1;
            }  
            testArray[i][j] = strtod(argv[argcCounter], &temp);
        }
    }
    else {
        // Random integers between 0 and RAND_MAX
        /*for (i = 0; i < arraySize; i++) {
            for (j = 0; j < arraySize; j++) {
                testArray[i][j] = rand();
            }
        }*/

        // 1.0s on outside
        for (i = 0; i < arraySize; i++) {
            for (j = 0; j < arraySize; j++) {
                if (i == 0 || j == 0 || i == arraySize - 1 || j == arraySize - 1) {
                    testArray[i][j] = 1.0;
                }
                else {
                    testArray[i][j] = 0.0;
                }
            }
        }
    }

    for (i = 0; i < arraySize; i++) {
        for (j = 0; j < arraySize; j++) {
            if (i == 0 || j == 0 || i == arraySize - 1 || j == arraySize - 1) {
                testArray[i][j] = 1.0;
            }
            else {
                testArray[i][j] = 0.0;
            }
        }
    }

    // A lot of this needs changing, including nested for loops, assigning values unnecessarily, and more
    int iterationNum = 0;

    // Here as does all calculations on 'old' values before updating, rather than
    double newArray[arraySize][arraySize];
    memcpy(newArray, testArray, arraySize * arraySize * sizeof(double));
    for (i = 0; i < arraySize; i++) {
        for (j = 0; j < arraySize; j++) {
            printf("%f\t", newArray[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    
    double biggestDiff;
    do {
        biggestDiff = 0.0;
        for (i = 0; i < arraySize; i++) {
            for (j = 0; j < arraySize; j++) {
                if (i != 0 && j != 0 && i != arraySize - 1 && j != arraySize - 1) {
                    double nums[4] = {testArray[i-1][j], testArray[i][j-1], testArray[i+1][j], testArray[i][j+1]};
                    double avg = meanOfFour(nums);
                    double diff = fabs(testArray[i][j] - avg);
                    newArray[i][j] = avg;
                    if (diff > biggestDiff) {
                        biggestDiff = diff;
                    }
                }
            }
        }
        iterationNum += 1;
        memcpy(testArray, newArray, arraySize * arraySize * sizeof(double));

        /*for (i = 0; i < arraySize; i++) {
            for (j = 0; j < arraySize; j++) {
                printf("%f\t", newArray[i][j]);
            }
            printf("\n");
        }
        printf("\n");*/

    } while (biggestDiff > precision); //comparison here as will already do at least once

    printf("After %d iterations:\n\n", iterationNum);
    /*for (i = 0; i < arraySize; i++) {
        for (j = 0; j < arraySize; j++) {
            printf("%f\t", testArray[i][j]);
        }
        printf("\n");
    }*/

    return 0;

}