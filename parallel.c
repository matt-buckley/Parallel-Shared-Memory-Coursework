#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
//#include <math.h>
#include <stdbool.h>

int arraySize;
double precision;
struct startFunctionArguments {
    int elementLoc;
    int elementsToProcess;
};
double **iterableArray;
double **finalArray;
bool precisionMetForAll;

// Can just combine this with the function above
void averageRows(void *arguments) {

    struct startFunctionArguments *args = (struct startFunctionArguments *) arguments;

    int elementLoc = args -> elementLoc;
    int elementsToProcess = args -> elementsToProcess;

    int row = ((elementLoc - 1) / (arraySize - 2)) + 1;
    int col = (elementLoc - ((row - 1) * (arraySize - 2)));

    while (elementsToProcess > 0) {

        iterableArray[row][col] = (finalArray[row - 1][col] + finalArray[row][col - 1] + finalArray[row + 1][col] + finalArray[row][col + 1]) / 4;

        if (precisionMetForAll == true) { // if 0, do nothing because at least one value is still > precision
            double diff = fabs(finalArray[row][col] - iterableArray[row][col]);
            if (diff > precision) {
                precisionMetForAll = false;
            }
        }

        col += 1;
        if (col == arraySize - 1) {
            row += 1;
            col = 1;
        }
        elementsToProcess--;

    }

}

void createThreads(int elementLoc, int elementsToProcess, pthread_t *threadArray, int numCurrentThreads) {
    pthread_t thread;

    struct startFunctionArguments *args = malloc(sizeof(struct startFunctionArguments));
    args -> elementLoc = elementLoc;
    args -> elementsToProcess = elementsToProcess;

    pthread_create(&thread, NULL, (void *(*)(void *))averageRows, (void *)args);
    
    threadArray[numCurrentThreads] = thread;
}

int main(int argc, char *argv[]) {

    arraySize = 18;
    precision = 0.001;
    // For iteration, must be defined before use in loop when using Balena's compiler.
    int row, col;

    // Define dimension
    if (argc > 1) {
        arraySize = atoi(argv[1]);
    }

    int numMutableElements = (arraySize - 2) * (arraySize - 2);

    // Define default thread count based on number of threads
    int numThreads = 16;

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




        // IN REPORT - MENTION THE ABOVE, AND MENTION WHAT THE DEFAULT IS
    
    
    
    }

    // A lot of this needs changing, including nested for loops, assigning values unnecessarily, and more
    int iterationNum = 0;

    finalArray = (double **) malloc(arraySize * sizeof(double *));
    for (row = 0; row < arraySize; row++) {
        finalArray[row] = (double *) malloc(arraySize * sizeof(double));
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
        row = -1;
        col = 0;
        char *temp;
        for (argcCounter = 4; argcCounter < argc; argcCounter++) {
            // Reset to start of next line
            if (((argcCounter - 4) % arraySize) == 0) {
                row += 1;
                col = 0;
            }
            else {
                col += 1;
            }  
            finalArray[row][col] = strtod(argv[argcCounter], &temp);
        }
    }
    else {
        // Random integers between 0 and RAND_MAX
        /*srand(5);
        for (row = 0; row < arraySize; row++) {
            for (col = 0; col < arraySize; col++) {
                finalArray[row][col] = rand() / (double) RAND_MAX;
            }
        }*/

        // 1.0s on outside
        for (row = 0; row < arraySize; row++) {
            for (col = 0; col < arraySize; col++) {
                if (row == 0 || col == 0 || row == arraySize - 1 || col == arraySize - 1) {
                    finalArray[row][col] = 1.0;
                }
                else {
                    finalArray[row][col] = 0.0;
                }
            }
        }
    }


    if (arraySize < 3) { // array has no mutable values
        for (row = 0; row < arraySize; row++) {
            for (col = 0; col < arraySize; col++) {
                printf("%f\t", finalArray[row][col]);
            }
        }
        printf("\n");
        return 0;
    }



    // Here as does all calculations on 'old' values before updating, rather than
    iterableArray = (double **) malloc(arraySize * sizeof(double *));
    for (row = 0; row < arraySize; row++) {
        iterableArray[row] = (double *) malloc(arraySize * sizeof(double));
    }

    for (row = 0; row < arraySize; row++) {
        for (col = 0; col < arraySize; col++) {
            if (row == 0 || col == 0 || row == arraySize - 1 || col == arraySize - 1) {
                iterableArray[row][col] = 1.0;
            }
            else {
                iterableArray[row][col] = 0.0;
            }
        }
    }

    /*for (row = 0; row < arraySize; row++) {
        for (col = 0; col < arraySize; col++) {
            printf("%f\t", iterableArray[row][col]);
        }
        printf("\n");
    }
    printf("\n");*/
    
    pthread_t threadArray[numThreads];
    int elementsPerThread;
    int numThreadsWithAnExtraElement;
    int numCurrentThreads;
    int elementLoc; // CAN REUSE row INSTEAD ONCE DEBUGGING COMPLETE

    if (numThreads > numMutableElements) {
        printf("Error - cannot be more threads than there are mutable elements in array. Reducing number of threads to this value.\n");
        numThreads = numMutableElements;
    }
    
    elementsPerThread = numMutableElements / numThreads;
    numThreadsWithAnExtraElement = numMutableElements % numThreads;

    do {

        numCurrentThreads = 0;
        numThreadsWithAnExtraElement = numMutableElements % numThreads; // is a caluclation or a memory fetch faster here?
        precisionMetForAll = true;

        // Only operates inside the mutable grid
        //printf("elementsPerThread: %d\n", elementsPerThread);
        //printf("numThreadsWithAnExtraELement: %d\n", numThreadsWithAnExtraElement);
        for (elementLoc = 1; elementLoc <= numMutableElements;) {
            if (elementLoc < 0) {
                printf("elementsPerThread: %d\n", elementsPerThread);
                printf("numThreadsWithAnExtraELement: %d\n", numThreadsWithAnExtraElement);
                printf("row: %d\n", elementLoc);
                exit(0);
            }
            if (numThreadsWithAnExtraElement > 0) {
                createThreads(elementLoc, elementsPerThread + 1, threadArray, numCurrentThreads);
                numThreadsWithAnExtraElement -= 1;
                elementLoc += (elementsPerThread + 1);
            }
            else {
                createThreads(elementLoc, elementsPerThread, threadArray, numCurrentThreads);
                elementLoc += elementsPerThread;
            }
            numCurrentThreads += 1;
        }

        for (row = 0; row < numCurrentThreads; row++) {
            pthread_join(threadArray[row], NULL);
        }

        iterationNum += 1;
        
        double **tmp = finalArray;
        finalArray = iterableArray;
        iterableArray = tmp;

        // To print array at the end of each iteration
        /*for (row = 0; row < arraySize; row++) {
            for (col = 0; col < arraySize; col++) {
                printf("%f\t", iterableArray[row][col]);
            }
            printf("\n");
        }
        printf("\n");*/


    } while (precisionMetForAll == false); //comparison here as will already do at least once

    printf("Completed after %d iterations using %d threads.\n", iterationNum, numCurrentThreads);
    FILE *file = fopen("resultParallel.txt", "w");
    for (row = 0; row < arraySize; row++) {
        for (col = 0; col < arraySize; col++) {
           fprintf(file, "%f,", finalArray[row][col]);
        }
        fprintf(file, "\n");
    }

    return 0;

}