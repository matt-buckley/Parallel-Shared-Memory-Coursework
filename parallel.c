#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
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
pthread_barrier_t barrier;

struct timespec startTime, endTime, startTotal, endTotal;

double sequentialTime;
double totalTime;

bool programEnd;

/**
 * Start function for pthreads - takes sequence of elements to process and fills iterable array
 * @param arguments: struct that contains location of first element to process and number of elements to process
 */
void averageRows(void *arguments) {

    struct startFunctionArguments *args = (struct startFunctionArguments *) arguments;

    int elementLoc = args -> elementLoc;
    int elementsToProcess = args -> elementsToProcess;

    // Uses elementLoc to calculate row and col of first element to process
    int initialRow = ((elementLoc - 1) / (arraySize - 2)) + 1;
    int initialCol = (elementLoc - ((initialRow - 1) * (arraySize - 2)));

    int row, col;

    // Thread continues operating until programEnd flag is true, or main() returns
    while (programEnd == false) {

        row = initialRow;
        col = initialCol;
        elementsToProcess = args -> elementsToProcess;

        // Continues processing elements one by one until all elements 'assigned' to this thread have been processed
        while (elementsToProcess > 0) {

            // Calculates mean of surrounding four elements and assigns to array
            // Reads from static array and assigns to mutable array
            iterableArray[row][col] = (finalArray[row - 1][col] + finalArray[row][col - 1] + finalArray[row + 1][col] + finalArray[row][col + 1]) / 4;

            // Precision checker — sets flag to false if difference between newly calculate value and 
            // previous value is greater than the precision
            // Only runs if flag is true, meaning either this is the first check, or all previous elements
            // are less than the precision
            if (precisionMetForAll == true) {
                double diff = fabs(finalArray[row][col] - iterableArray[row][col]);
                if (diff > precision) {
                    precisionMetForAll = false;
                }
            }

            // Updates row and col value for next element
            col += 1;
            if (col == arraySize - 1) {
                row += 1;
                col = 1;
            }
            elementsToProcess--;

        }

        // Blocks thread to ensure all other threads are finished before continuing
        pthread_barrier_wait(&barrier);
        // Blocks thread again to allow main to update arrays and check if finished
        pthread_barrier_wait(&barrier);

    }

}

/**
 * Creates struct of arguments to pass, then creates thread
 * @param elementLoc: 'Location' of first element for thread to process (e.g. [1][1] = 1, [2][1] = arraySize-1)
 * @param elementsToProcess: Number of elements this thread should process from elementLoc onwards
 */
void createThreads(int elementLoc, int elementsToProcess) {

    pthread_t thread;

    // Creates struct to hold arguments that will be passed to start function of new thread
    struct startFunctionArguments *args = malloc(sizeof(struct startFunctionArguments));
    args -> elementLoc = elementLoc;
    args -> elementsToProcess = elementsToProcess;

    pthread_create(&thread, NULL, (void *(*)(void *))averageRows, (void *)args);
    
}

/**
 * Main function, handles most serial processing
 * @param argc: Number of arguments passed to main from command line
 * @param argv: Arguments passed to main
 */
int main(int argc, char *argv[]) {



    // STILL NEED TO COMMENT CLOCK FUNCTIONS


    clock_gettime(CLOCK_MONOTONIC, &startTotal);
    clock_gettime(CLOCK_MONOTONIC, &startTime);




    programEnd = false;

    // Define default array dimension, precision, and thread count
    arraySize = 18;
    precision = 0.01;
    int numThreads = 16;

    // Define default values as strings for output filename
    char *numThreadsStr = malloc(20 * sizeof(char));
    numThreadsStr = "16";
    char *arraySizeStr = malloc(20 * sizeof(char));
    arraySizeStr = "18";

    // For iteration, but these must be defined before use in loop when using Balena's compiler.
    int row, col;

    // Read in array dimension if defined in command line
    if (argc > 1) {
        arraySize = atoi(argv[1]);
        arraySizeStr = argv[1];
    }

    // Number of elements to change — does not include boundary elements
    int numMutableElements = (arraySize - 2) * (arraySize - 2);
    if (numMutableElements < 1) {
        printf("Error — array too small to iterate on.");
        return 0;
    }

    // Read in precision if defined in command line
    if (argc > 2) {
        char *temp;
        precision = strtod(argv[2], &temp);
    }

    // Read in number of threads to use if defined in command line
    if (argc > 3) {
        numThreads = atoi(argv[3]);
        numThreadsStr = argv[3];
        if (numThreads < 1) {
            printf("Invalid number of threads.");
            return 0;
        }
    }

    // Reduces number of threads if more threads are assigned than could be used
    if (numThreads > numMutableElements) {
        printf("Cannot use this many threads (default 16) as there are more threads than there are mutable elements in array. Reducing number of threads to number of mutable elements.\n");
        numThreads = numMutableElements;
        numThreadsStr = "unknown number of";
    }

    // Assign memory for final array (holds final values and is static during iteration)
    finalArray = (double **) malloc(arraySize * sizeof(double *));
    for (row = 0; row < arraySize; row++) {
        finalArray[row] = (double *) malloc(arraySize * sizeof(double));
    }
    
    // Read in array from file if filename defined in command line
    if (argc > 4) {

        printf("Reading in file. Errors may occur if array stored in file is not %dx%d.\n", arraySize, arraySize);
        
        FILE *file = fopen(argv[4], "r");
        char *tempChar;
        char *buff = malloc(20 * sizeof(char)); // 20 chars should be more than enough to hold the number of decimal points a C double can possess (IEEE 754 encoding)
        
        for (row = 0; row < arraySize; row++) {
            for (col = 0; col < arraySize; col++) {
                fscanf(file, "%s ", buff);
                finalArray[row][col] = strtod(buff, &tempChar);
            }
        }

        fclose(file);

    }
    // If no filename entered into command line, generate default array
    else {
        
        // Random seed is defined to be the same as sequential program to ensure same array is generated
        // To change the random seed, alter the parameter passed to srand()
        srand(1);

        // Default array is random 1.0s and 0.0s
        for (row = 0; row < arraySize; row++) {
            for (col = 0; col < arraySize; col++) {
                finalArray[row][col] = rand() % arraySize;
            }
        }

    }

    // Assigns memory for array newly calculate values are written to
    iterableArray = (double **) malloc(arraySize * sizeof(double *));
    for (row = 0; row < arraySize; row++) {
        iterableArray[row] = (double *) malloc(arraySize * sizeof(double));
    }

    // Sets the outer elements of iterableArray as the inner elements will be filled during the first iteration
    for (row = 0; row < arraySize; row++) {
        for (col = 0; col < arraySize; col++) {
            if (row == 0 || col == 0 || row == arraySize - 1 || col == arraySize - 1) {
                iterableArray[row][col] = finalArray[row][col];
            }
        }
    }
    
    // Each thread will process at least 'elementsPerThread' elements each iteration, 
    // with 'numThreadsWithAnExtraElement' each processing one extra element each
    // Work is hence as evenly divided between the threads as possible
    int elementsPerThread;
    int numThreadsWithAnExtraElement;
    elementsPerThread = numMutableElements / numThreads;
    numThreadsWithAnExtraElement = numMutableElements % numThreads;

    // Creates barrier with count of total pthreads plus control thread (the thread running main())
    pthread_barrier_init(&barrier, NULL, numThreads + 1);

    int elementLoc;

    // Iteration counter
    int iterationNum = 0;
    
    // Precision flag — true if program can finish as criteria fulfilled
    // Precision flag set to true here so threads will know if they need to check whether to set it
    precisionMetForAll = true;





    // Will miss thread creation but ah well
    // COMMENT




    clock_gettime(CLOCK_MONOTONIC, &endTime);
    sequentialTime = (endTime.tv_sec - startTime.tv_sec) * 1000000000;
    sequentialTime += (endTime.tv_nsec - startTime.tv_nsec);




    // Creates threads according to specifications defined by variables above
    for (elementLoc = 1; elementLoc <= numMutableElements;) {

        // Creates 'numThreadsWithAnExtraElement' pthreads that will process one extra element
        if (numThreadsWithAnExtraElement > 0) {
            createThreads(elementLoc, elementsPerThread + 1);
            numThreadsWithAnExtraElement -= 1;
            elementLoc += (elementsPerThread + 1);
        }

        // Creates the remaining pthreads to process the default number of elements
        else {
            createThreads(elementLoc, elementsPerThread);
            elementLoc += elementsPerThread;
        }

    }

    // Loop that updates arrays between iterations and checks if program should finish, , when all elements change by less than the precision
    do {

        // Resets precision flag between iterations and calls wait once arrays have been updated
        // to allow threads to continue to next iteration
        // Reset is here to allow precision check (the 'while') to complete before continuing
        if (iterationNum != 0) {
            precisionMetForAll = true;

            // ADD THIS BIT TO COMMENTS
            clock_gettime(CLOCK_MONOTONIC, &endTime);
            sequentialTime += (endTime.tv_sec - startTime.tv_sec) * 1000000000;
            sequentialTime += (endTime.tv_nsec - startTime.tv_nsec);
            
            pthread_barrier_wait(&barrier);
        }

        // Ensures all threads have finished processing this iteration before continuing
        pthread_barrier_wait(&barrier);

        // COMMENT THIS
        clock_gettime(CLOCK_MONOTONIC, &startTime);

        iterationNum += 1;
        
        // Swaps (pointers to) static and iterable arrays between iterations to update readable values
        // to those just calculated
        double **tmp = finalArray;
        finalArray = iterableArray;
        iterableArray = tmp;

        // If precisionMetForAll is false, continue iterating; else, finalise program

    } while (precisionMetForAll == false);

    // Not technically needed as program will end and free threads regardless, but avoids using an infinite loop
    programEnd = true;

    //printf("Completed after %d iterations using %d threads.\n", iterationNum, numThreads);
    
    // Prints output to file for correctness testing
    char filename[25] = "resultParallel-";
    strcat(filename, numThreadsStr);
    strcat(filename, "-");
    strcat(filename, arraySizeStr);
    strcat(filename, ".txt");
    FILE *file = fopen(filename, "w");
    for (row = 0; row < arraySize; row++) {
        for (col = 0; col < arraySize; col++) {
           fprintf(file, "%f,", finalArray[row][col]);
        }
        fprintf(file, "\n");
    }

    // COMMENT THIS
    clock_gettime(CLOCK_MONOTONIC, &endTotal);
    totalTime = (endTotal.tv_sec - startTotal.tv_sec) * 1000000000;
    totalTime += (endTotal.tv_nsec - startTotal.tv_nsec);
    printf("Total sequential time %f seconds\n", sequentialTime / 1000000000);
    printf("Total time: %f seconds\n", totalTime / 1000000000);

    return 0;

}