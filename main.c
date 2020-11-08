#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <pthread.h>

int arraySize;
double precision;
struct startFunctionArguments {
    int i, j;
};
double **newArray;
double **testArray;

// This is always going to be 4, and passing more arguments to make it reusable would increase unnecessary communication
// Should this even be in a function?
double meanOfFour(int i, int j, int test) {
    
    double sum = testArray[i-1][j] + testArray[i][j-1] + testArray[i+1][j] + testArray[i][j+1]; //optimise this

    if (test != 1) {
        newArray[i][j] = sum / 4;
    } else {
        return newArray[i][j];
    }

    //return sum / 4; // calculating again should be faster than fetching from memory
}

void *startFunction(void *arguments) {

    struct startFunctionArguments *args = (struct startFunctionArguments *) arguments;

    int i = args -> i;
    int j = args -> j;

    meanOfFour(i, j, 1);
}

void createThreads(int i, int j) {
    pthread_t thread;

    struct startFunctionArguments *args = malloc(sizeof(struct startFunctionArguments));
    args -> i = i;
    args -> j = j;

    pthread_create(&thread, NULL, startFunction, (void *)args);
}

int main(int argc, char *argv[]) {

    // Any way to make these const?
    arraySize = 5;
    precision = 0.001;
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

    // A lot of this needs changing, including nested for loops, assigning values unnecessarily, and more
    int iterationNum = 0;

    testArray = (double **) malloc(arraySize * sizeof(double *));
    for (i = 0; i < arraySize; i++) {
        testArray[i] = (double *) malloc(arraySize * sizeof(double));
    }
    //double testArray[arraySize][arraySize];
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

    // Here as does all calculations on 'old' values before updating, rather than
    newArray = (double **) malloc(arraySize * sizeof(double *));
    for (i = 0; i < arraySize; i++) {
        newArray[i] = (double *) malloc(arraySize * sizeof(double));
    }

    //double newArray[arraySize][arraySize];
    // Can't use memcpy() as this copies the pointers rather than the values
    //memcpy(newArray, testArray, arraySize * arraySize * sizeof(double));
    for (i = 0; i < arraySize; i++) {
        for (j = 0; j < arraySize; j++) {
            newArray[i][j] = testArray[i][j];
        }
    }

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
        for (i = 1; i < arraySize - 1; i++) {
            for (j = 1; j < arraySize - 1; j++) {
                if (i == 2 && j == 2 && iterationNum == 1) {
                    createThreads(i, j);
                }
                meanOfFour(i, j, 0);
                // NEEDS TO BE DONE AT THE END OF THE ITERATION WHEN IN PARALLEL
                double diff = fabs(testArray[i][j] - newArray[i][j]);
                if (diff > biggestDiff) {
                    biggestDiff = diff;
                }
            }
        }
        iterationNum += 1;
        //memcpy(testArray, newArray, arraySize * arraySize * sizeof(double));
        for (i = 0; i < arraySize; i++) {
            for (j = 0; j < arraySize; j++) {
                testArray[i][j] = newArray[i][j];
            }
        }

        for (i = 0; i < arraySize; i++) {
            for (j = 0; j < arraySize; j++) {
                printf("%f\t", newArray[i][j]);
            }
            printf("\n");
        }
        printf("\n");

    } while (biggestDiff > precision); //comparison here as will already do at least once

    printf("After %d iterations:\n\n", iterationNum);
    for (i = 0; i < arraySize; i++) {
        for (j = 0; j < arraySize; j++) {
            printf("%f\t", testArray[i][j]);
        }
        printf("\n");
    }

    return 0;

}