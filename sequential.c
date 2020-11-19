#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

// This is always going to be 4, and passing more arguments to make it reusable would increase unnecessary communication
// Should this even be in a function?
double meanOfFour(double *nums, double **iterableArray) {
    double sum = 0;

    int row;
    for (row = 0; row < 4; row++) {
        sum += nums[row];
    }

    return sum / 4;
}

int main(int argc, char *argv[]) {

    // Any way to make these const?
    int arraySize = 18;
    char *arraySizeStr = malloc(10 * sizeof(char));
    arraySizeStr = "18";
    double precision = 0.001;
    // For iteration
    int row, col;

    // Define dimension
    if (argc > 1) {
        arraySize = atoi(argv[1]);
        arraySizeStr = argv[1];
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
    for (row = 0; row < arraySize; row++) {
        finalArray[row] = (double *) malloc(arraySize * sizeof(double));
    }

    if (argc > 3) {
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
        for (argcCounter = 3; argcCounter < argc; argcCounter++) {
            // Reset to start of next line
            if (((argcCounter - 3) % arraySize) == 0) {
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
        srand(10); // same random seed as parallel.c
        for (row = 0; row < arraySize; row++) {
            for (col = 0; col < arraySize; col++) {
                finalArray[row][col] = rand() % 2;
            }
        }

        // 1.0s on outside
        /*for (row = 0; row < arraySize; row++) {
            for (col = 0; col < arraySize; col++) {
                if (row == 0 || col == 0 || row == arraySize - 1 || col == arraySize - 1) {
                    finalArray[row][col] = 1.0;
                }
                else {
                    finalArray[row][col] = 0.0;
                }
            }
        }*/

    }

    // A lot of this needs changing, including nested for loops, assigning values unnecessarily, and more
    int iterationNum = 0;

    // Here as does all calculations on 'old' values before updating, rather than
    double **iterableArray = (double **) malloc(arraySize * sizeof(double *));
    for (row = 0; row < arraySize; row++) {
        iterableArray[row] = (double *) malloc(arraySize * sizeof(double));
    }


    // Only need to set the outer elements as the inner elements will be filled during the first iteration
    for (row = 0; row < arraySize; row++) {
        for (col = 0; col < arraySize; col++) {
            if (row == 0 || col == 0 || row == arraySize - 1 || col == arraySize - 1) {
                iterableArray[row][col] = finalArray[row][col];
            }
        }
    }
    
    bool precisionMetForAll;
    int k;
    do {
        precisionMetForAll = true;
        for (row = 1; row < arraySize - 1; row++) {
            for (col = 1; col < arraySize - 1; col++) {
                
                iterableArray[row][col] = (finalArray[row - 1][col] + finalArray[row][col - 1] + finalArray[row + 1][col] + finalArray[row][col + 1]) / 4;

                if (precisionMetForAll == true) { // if 0, do nothing because at least one value is still > precision
                    if (fabs(finalArray[row][col] - iterableArray[row][col]) > precision) {
                        precisionMetForAll = false;
                    }
                }
            }
        }
        iterationNum += 1;
        
        double **tmp = finalArray;
        finalArray = iterableArray;
        iterableArray = tmp;

        /*for (row = 0; row < arraySize; row++) {
            for (col = 0; col < arraySize; col++) {
                printf("%f\t", iterableArray[row][col]);
            }
            printf("\n");
        }
        printf("\n");*/

    } while (precisionMetForAll == false); //comparison here as will already do at least once

    // ONLY NEEDED FOR CORRECTNESS TESTING
    /*printf("Completed sequentially after %d iterations.\n", iterationNum);
    char filename[25] = "resultSequential-";
    strcat(filename, arraySizeStr);
    strcat(filename, ".txt");
    FILE *file = fopen(filename, "w");
    for (row = 0; row < arraySize; row++) {
        for (col = 0; col < arraySize; col++) {
            fprintf(file, "%f,", finalArray[row][col]);
        }
        fprintf(file, "\n");
    }*/

    return 0;

}