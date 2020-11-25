#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

/**
 * Main function, handles all processing; does not use other functions to reduce calls
 * @param argc: Number of arguments passed to main from command line
 * @param argv: Arguments passed to main
 */
int main(int argc, char *argv[]) {

    // Define default array dimension, precision, and thread count
    int arraySize = 18;
    double precision = 0.01;

    // Define default values as strings for output filename
    char *arraySizeStr = malloc(20 * sizeof(char));
    arraySizeStr = "18";

    // For iteration, but these must be defined before use in loop when using Balena's compiler.
    int row, col;

    // Read in array dimension if defined in command line
    if (argc > 1) {
        arraySize = atoi(argv[1]);
        arraySizeStr = argv[1];
    }

    // Read in precision if defined in command line
    if (argc > 2) {
        char *temp;
        precision = strtod(argv[2], &temp);
    }
    
    // Assign memory for final array (holds final values and is static during iteration)
    double **finalArray = (double **) malloc(arraySize * sizeof(double *));
    for (row = 0; row < arraySize; row++) {
        finalArray[row] = (double *) malloc(arraySize * sizeof(double));
    }

    // Read in array from file if filename defined in command line
    if (argc > 3) {
        
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
    double **iterableArray = (double **) malloc(arraySize * sizeof(double *));
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

    // Iteration counter
    int iterationNum = 0;
    
    // Precision flag - true if program can finalise
    bool precisionMetForAll;

    // Loop that iterates until program should finish, when all elements change by less than the precision
    do {

        precisionMetForAll = true;

        // Loops through mutable part of the array
        for (row = 1; row < arraySize - 1; row++) {
            for (col = 1; col < arraySize - 1; col++) {
                
                // Calculates mean of surrounding four elements and assigns to array
                // Reads from static array and assigns to mutable array
                iterableArray[row][col] = (finalArray[row - 1][col] + finalArray[row][col - 1] + finalArray[row + 1][col] + finalArray[row][col + 1]) / 4;

                // Precision checker — sets flag to false if difference between newly calculate value and 
                // previous value is greater than the precision
                // Only runs if flag is true, meaning either this is the first check, or all previous elements
                // are less than the precision
                if (precisionMetForAll == true) {
                    if (fabs(finalArray[row][col] - iterableArray[row][col]) > precision) {
                        precisionMetForAll = false;
                    }
                }
            }
        }

        iterationNum += 1;
        
        // Swaps (pointers to) static and iterable arrays between iterations to update readable values
        // to those just calculated
        double **tmp = finalArray;
        finalArray = iterableArray;
        iterableArray = tmp;

        // If precisionMetForAll is false, continue iterating; else, finalise program

    } while (precisionMetForAll == false);

    // Prints output to file for correctness testing
    printf("Completed sequentially after %d iterations.\n", iterationNum);
    char filename[25] = "resultSequential-";
    strcat(filename, arraySizeStr);
    strcat(filename, ".txt");
    FILE *file = fopen(filename, "w");
    for (row = 0; row < arraySize; row++) {
        for (col = 0; col < arraySize; col++) {
            fprintf(file, "%f,", finalArray[row][col]);
        }
        fprintf(file, "\n");
    }

    return 0;

}