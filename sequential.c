#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc > 1) {
        const int arraySize = atoi(argv[1]);
    }
    // Change to use malloc
    //double testArray[arraySize][arraySize];

    // HARDCODED
    double testArray[5][5] = {
        {1.0, 1.0, 1.0, 1.0, 1.0},
        {1.0, 0.0, 0.0, 0.0, 1.0},
        {1.0, 0.0, 0.0, 0.0, 1.0},
        {1.0, 0.0, 0.0, 0.0, 1.0},
        {1.0, 1.0, 1.0, 1.0, 1.0}
    };


}