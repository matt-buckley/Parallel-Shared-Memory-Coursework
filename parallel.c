#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
//#include <math.h>

int arraySize;
double precision;
struct startFunctionArguments {
    int elementLoc;
    int elementsToProcess;
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

    int elementLoc = args -> elementLoc;
    int elementsToProcess = args -> elementsToProcess;

    //printf("elementLoc: %d\n", elementLoc);
    //printf("elementsToProcess: %d\n", elementsToProcess);
    int row = ((elementLoc - 1) / (arraySize - 2)) + 1;
    int col = (elementLoc - ((row - 1) * (arraySize - 2)));
    //printf("Initial row: %d\n", row);
    //printf("Initial column: %d\n", col);
    //printf("\n");

    while (elementsToProcess > 0) {
        //if (row > arraySize - 2 || col > arraySize - 2) {
        /*    printf("elementLoc: %d\n", args -> elementLoc);
            printf("elementsToProcess: %d\n", args -> elementsToProcess);
            printf("Initial row: %d\n", ((elementLoc - 1) / (arraySize - 2)) + 1);
            printf("Initial column: %d\n", (elementLoc - ((((elementLoc - 1) / (arraySize - 2)) + 1) - 1) * (arraySize - 2)));
            printf("Row: %d\n", row);
            printf("Column: %d\n\n", col);
        *///}
        double sum = testArray[row - 1][col] + testArray[row][col - 1] + testArray[row + 1][col] + testArray[row][col + 1];
        newArray[row][col] = sum / 4;

        col += 1;
        if (col == arraySize - 1) {
            row += 1;
            col = 1;
        }
        elementsToProcess--;
    }

    /*int currentRow;
    int currentCol;
    for (currentRow = row; currentRow < row + rowsToProcess; currentRow++) {
        for (col = 1; col < arraySize - 1; col++) {
            double sum = testArray[currentRow - 1][col] + testArray[currentRow][col - 1] + testArray[currentRow + 1][col] + testArray[currentRow][col + 1];
            newArray[currentRow][col] = sum / 4;
        }
    }*/

    //pthread_barrier_wait(args -> barrier);
}

void createThreads(int elementLoc, int elementsToProcess, pthread_t *threadArray, int numCurrentThreads) {
    pthread_t thread;

    struct startFunctionArguments *args = malloc(sizeof(struct startFunctionArguments));
    args -> elementLoc = elementLoc;
    args -> elementsToProcess = elementsToProcess;

    pthread_create(&thread, NULL, averageRows, (void *)args);

    threadArray[numCurrentThreads] = thread;
    
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

    // Define default thread count based on number of threads
    int numThreads = arraySize - 2;

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
    
    // Still need to ensure that:
    // - Full number of array elements is entered
    // - Entered array matches given array dimensions
    // Maybe just mention it may not work if incorrect in report
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

    /*for (i = 0; i < arraySize; i++) {
        for (j = 0; j < arraySize; j++) {
            printf("%f\t", newArray[i][j]);
        }
        printf("\n");
    }
    printf("\n");*/
    
    double biggestDiff;
    pthread_t threadArray[numThreads];
    int elementsPerThread;
    int numThreadsWithAnExtraElement;
    int numCurrentThreads;
    int elementLoc; // CAN REUSE i INSTEAD ONCE DEBUGGING COMPLETE

    if (numThreads > (arraySize - 2) * (arraySize - 2)) {
        printf("Error - cannot be more threads than there are mutable elements in array. Reducing number of threads to this value.\n");
        numThreads = (arraySize - 2) * (arraySize - 2);
    }
    
    elementsPerThread = (arraySize - 2) * (arraySize - 2) / numThreads;
    numThreadsWithAnExtraElement = (arraySize - 2) * (arraySize - 2) % numThreads;

    //pthread_barrier_t barrier;
    //pthread_barrier_init(&barrier, NULL, arraySize - 1);
    do {

        numCurrentThreads = 0;
        numThreadsWithAnExtraElement = (arraySize - 2) * (arraySize - 2) % numThreads; // is a caluclation or a memory fetch faster here?

        // Only operates inside the mutable grid
        //printf("elementsPerThread: %d\n", elementsPerThread);
        //printf("numThreadsWithAnExtraELement: %d\n", numThreadsWithAnExtraElement);
        for (elementLoc = 1; elementLoc <= ((arraySize - 2) * (arraySize - 2));) {
            if (elementLoc < 0) {
                printf("elementsPerThread: %d\n", elementsPerThread);
                printf("numThreadsWithAnExtraELement: %d\n", numThreadsWithAnExtraElement);
                printf("i: %d\n", elementLoc);
                exit(0);
            }
            if (numThreadsWithAnExtraElement > 0) {
                createThreads(elementLoc, elementsPerThread + 1, threadArray, numCurrentThreads);
                numThreadsWithAnExtraElement -= 1;
                elementLoc += (elementsPerThread + 1);
            }
            else {
                //printf("%d\n", numCurrentThreads);
                //printf("before createThreads: %d\n", elementLoc);
                //printf("elementsPerThread before createThreads: %d\n", elementsPerThread);
                //printf("Address of i in main: %p\n", &elementLoc);
                createThreads(elementLoc, elementsPerThread, threadArray, numCurrentThreads);
                //printf("before incrementing i: %d\n", elementLoc);
                //printf("elementsPerThread before incrementing i: %d\n", elementsPerThread);
                elementLoc += elementsPerThread;
                //printf("else: %d\n\n", elementLoc);
                //exit(0);
            }
            numCurrentThreads += 1;
        }

        //pthread_barrier_wait(&barrier);
        // BARRIER WOULD BE BETTER HERE
        for (i = 0; i < numCurrentThreads; i++) {
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

    printf("Completed after %d iterations using %d threads\n", iterationNum, numCurrentThreads);
    /*for (i = 0; i < arraySize; i++) {
        for (j = 0; j < arraySize; j++) {
            printf("%f\t", testArray[i][j]);
        }
        printf("\n");
    }*/

    return 0;

}