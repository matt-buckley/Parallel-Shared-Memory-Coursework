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

// This is always going to be 4, and passing more arguments to make it reusable would increase unnecessary communication
// Should this even be in a function?
/*double meanOfFour(int i, int j, int test) {
    
    double sum = finalArray[i-1][j] + finalArray[i][j-1] + finalArray[i+1][j] + finalArray[i][j+1]; //optimise this

    if (test != 1) {
        iterableArray[i][j] = sum / 4;
    } else {
        return iterableArray[i][j];
    }

    //return sum / 4; // calculating again should be faster than fetching from memory
}*/

// Can just combine this with the function above
void *averageRows(void *arguments) {

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

    arraySize = 18;
    precision = 0.001;
    // For iteration, must be defined before use in loop when using Balena's compiler.
    int i, j;

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
    for (i = 0; i < arraySize; i++) {
        finalArray[i] = (double *) malloc(arraySize * sizeof(double));
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


    if (arraySize < 3) { // array has no mutable values
        for (i = 0; i < arraySize; i++) {
            for (j = 0; j < arraySize; j++) {
                printf("%f\t", finalArray[i][j]);
            }
        }
        printf("\n");
        return 0;
    }



    // Here as does all calculations on 'old' values before updating, rather than
    iterableArray = (double **) malloc(arraySize * sizeof(double *));
    for (i = 0; i < arraySize; i++) {
        iterableArray[i] = (double *) malloc(arraySize * sizeof(double));
    }

    for (i = 0; i < arraySize; i++) {
        for (j = 0; j < arraySize; j++) {
            iterableArray[i][j] = finalArray[i][j];
        }
    }

    /*for (i = 0; i < arraySize; i++) {
        for (j = 0; j < arraySize; j++) {
            printf("%f\t", iterableArray[i][j]);
        }
        printf("\n");
    }
    printf("\n");*/
    
    pthread_t threadArray[numThreads];
    int elementsPerThread;
    int numThreadsWithAnExtraElement;
    int numCurrentThreads;
    int elementLoc; // CAN REUSE i INSTEAD ONCE DEBUGGING COMPLETE

    if (numThreads > numMutableElements) {
        printf("Error - cannot be more threads than there are mutable elements in array. Reducing number of threads to this value.\n");
        numThreads = numMutableElements;
    }
    
    elementsPerThread = numMutableElements / numThreads;
    numThreadsWithAnExtraElement = numMutableElements % numThreads;

    //pthread_barrier_t barrier;
    //pthread_barrier_init(&barrier, NULL, arraySize - 1);
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

        iterationNum += 1;
        
        double **tmp = finalArray;
        finalArray = iterableArray;
        iterableArray = tmp;

        // To print array at the end of each iteration
        /*for (i = 0; i < arraySize; i++) {
            for (j = 0; j < arraySize; j++) {
                printf("%f\t", iterableArray[i][j]);
            }
            printf("\n");
        }
        printf("\n");*/


    } while (precisionMetForAll == false); //comparison here as will already do at least once

    printf("Completed after %d iterations using %d threads.\n", iterationNum, numCurrentThreads);
    FILE *file = fopen("resultParallel.txt", "w");
    for (i = 0; i < arraySize; i++) {
        for (j = 0; j < arraySize; j++) {
           fprintf(file, "%f,", finalArray[i][j]);
        }
        fprintf(file, "\n");
    }

    return 0;

}