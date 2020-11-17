#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// This is always going to be 4, and passing more arguments to make it reusable would increase unnecessary communication
// Should this even be in a function?
double meanOfFour(double *nums, double **iterableArray) {
    double sum = 0;

    int i;
    for (i = 0; i < 4; i++) {
        sum += nums[i];
    }

    return sum / 4;
}

int main(int argc, char *argv[]) {

    // Any way to make these const?
    int arraySize = 18;
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
    
    double **finalArray = (double **) malloc(arraySize * sizeof(double *));
    for (i = 0; i < arraySize; i++) {
        finalArray[i] = (double *) malloc(arraySize * sizeof(double));
    }

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
            finalArray[i][j] = strtod(argv[argcCounter], &temp);
        }
    }
    else {
        // Random integers between 0 and RAND_MAX
        /*srand(5);
        for (i = 0; i < arraySize; i++) {
            for (j = 0; j < arraySize; j++) {
                finalArray[i][j] = rand() / (double) RAND_MAX;
            }
        }*/

        // 1.0s on outside
        for (i = 0; i < arraySize; i++) {
            for (j = 0; j < arraySize; j++) {
                if (i == 0 || j == 0 || i == arraySize - 1 || j == arraySize - 1) {
                    finalArray[i][j] = 1.0;
                }
                else {
                    finalArray[i][j] = 0.0;
                }
            }
        }
    }

    // A lot of this needs changing, including nested for loops, assigning values unnecessarily, and more
    int iterationNum = 0;

    // Here as does all calculations on 'old' values before updating, rather than
    double **iterableArray = (double **) malloc(arraySize * sizeof(double *));
    for (i = 0; i < arraySize; i++) {
        iterableArray[i] = (double *) malloc(arraySize * sizeof(double));
    }

    for (i = 0; i < arraySize; i++) {
        for (j = 0; j < arraySize; j++) {
            iterableArray[i][j] = finalArray[i][j];
        }
    }
    
    double biggestDiff;
    int k;
    do {
        biggestDiff = 0.0;
        for (i = 1; i < arraySize - 1; i++) {
            for (j = 1; j < arraySize - 1; j++) {
                
                iterableArray[i][j] = (finalArray[row - 1][col] + finalArray[row][col - 1] + finalArray[row + 1][col] + finalArray[row][col + 1]) / 4;

                double diff = fabs(finalArray[i][j] - iterableArray[i][j]);
                if (diff > biggestDiff) {
                    biggestDiff = diff;
                }
            }
        }
        iterationNum += 1;
        
        double **tmp = finalArray;
        finalArray = iterableArray;
        iterableArray = tmp;

        /*for (i = 0; i < arraySize; i++) {
            for (j = 0; j < arraySize; j++) {
                printf("%f\t", iterableArray[i][j]);
            }
            printf("\n");
        }
        printf("\n");*/

    } while (biggestDiff > precision); //comparison here as will already do at least once

    printf("Completed sequentially after %d iterations.\n", iterationNum);
    FILE *file = fopen("resultSequential.txt", "w");
    for (i = 0; i < arraySize; i++) {
        for (j = 0; j < arraySize; j++) {
            fprintf(file, "%f,", finalArray[i][j]);
        }
        fprintf(file, "\n");
    }

    return 0;

}