#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
//#include <math.h>

int arraySize;
double precision;
struct startFunctionArguments {
    int row;
    int rowsToProcess;
};
double **newArray;
double **testArray;

// This is always going to be 4, and passing more arguments to make it reusable would increase unnecessary communication
// Should this even be in a function?
/*double meanOfFour(int i, int j, int test) {
    
    double sum = testArray[i-1][j] + testArray[i][j-1] + testArray[i+1][j] + testArray[i][j+1]; //optimise this

    if (test != 1) {
        newArray[i][j] = sum / 4;
    } else {
        return newArray[i][j];
    }

    //return sum / 4; // calculating again should be faster than fetching from memory
}*/

// Can just combine this with the function above
void *averageRows(void *arguments) {

    struct startFunctionArguments *args = (struct startFunctionArguments *) arguments;

    int col;
    int row = args -> row;
    int rowsToProcess = args -> rowsToProcess;

    int currentRow;
    for (currentRow = row; currentRow < row + rowsToProcess; currentRow++) {
        for (col = 1; col < arraySize - 1; col++) {
            double sum = testArray[currentRow - 1][col] + testArray[currentRow][col - 1] + testArray[currentRow + 1][col] + testArray[currentRow][col + 1];
            newArray[currentRow][col] = sum / 4;
        }
    }

    //pthread_barrier_wait(args -> barrier);
}

void createThreads(int row, int rowsToProcess, pthread_t *threadArray, int numCurrentThreads) {
    pthread_t thread;

    struct startFunctionArguments *args = malloc(sizeof(struct startFunctionArguments));
    args -> row = row;
    args -> rowsToProcess = rowsToProcess;

    pthread_create(&thread, NULL, averageRows, (void *)args);

    threadArray[numCurrentThreads] = thread;
}

int main(int argc, char *argv[]) {

    // Any way to make these const?
    arraySize = 5;
    precision = 0.001;
    int numThreads = arraySize - 2;
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

    if (argc > 3) {
        char *temp;
        numThreads = atoi(argv[3]);
        if (numThreads == 0) {
            printf("Invalid number of threads - please enter -1 to use the default");
            return 0;
        }
        if (numThreads == -1) {
            numThreads = arraySize - 2;
        }




        // IN REPORT - MENTION THE ABOVE, AND MENTION WHAT THE DEFAULT IS
    
    
    
    }

    // A lot of this needs changing, including nested for loops, assigning values unnecessarily, and more
    int iterationNum = 0;

    testArray = (double **) malloc(arraySize * sizeof(double *));
    for (i = 0; i < arraySize; i++) {
        testArray[i] = (double *) malloc(arraySize * sizeof(double));
    }
    
    if (argc > 4) {
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
        for (argcCounter = 4; argcCounter < argc; argcCounter++) {
            // Reset to start of next line
            if (((argcCounter - 4) % arraySize) == 0) {
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
    pthread_t threadArray[arraySize - 2];
    int rowsPerThread;
    int numThreadsWithAnExtraRow;
    int numCurrentThreads;

    if (numThreads <= arraySize - 2) {
        rowsPerThread = (arraySize - 2) / numThreads;
        numThreadsWithAnExtraRow = (arraySize - 2) % numThreads;
        // Split up columns too
    }
    // TODO: handle if numThreads > number of rows to process

    //pthread_barrier_t barrier;
    //pthread_barrier_init(&barrier, NULL, arraySize - 1);
    do {

        numCurrentThreads = 0;

        for (i = 1; i < arraySize - 1;) {
            if (numThreadsWithAnExtraRow > 0) {
                createThreads(i, rowsPerThread + 1, threadArray, numCurrentThreads);
                numThreadsWithAnExtraRow -= 1;
                i += (rowsPerThread + 1);
            }
            else {
                createThreads(i, rowsPerThread, threadArray, numCurrentThreads);
                i += rowsPerThread;
            }
            numCurrentThreads += 1;
        }

        numThreadsWithAnExtraRow = (arraySize - 2) % numThreads; // is a caluclation or a memory fetch faster here?

        //pthread_barrier_wait(&barrier);
        // BARRIER WOULD BE BETTER HERE
        for (i = 0; i < numThreads; i++) {
            pthread_join(threadArray[i], NULL);
        }

        // Precision checker
        biggestDiff = 0.0;
        for (i = 1; i < arraySize - 1; i++) {
            for (j = 1; j < arraySize - 1; j++) {
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

/*
        for (i = 0; i < arraySize; i++) {
            for (j = 0; j < arraySize; j++) {
                printf("%f\t", newArray[i][j]);
            }
            printf("\n");
        }
        printf("\n");
*/

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