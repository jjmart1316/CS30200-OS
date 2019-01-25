/*
 * Course: CS 30200
 * Assignment: 1
 * */

#include <stdio.h>
#include <stdlib.h>

#define WIDTH 4 // 4 = default value (width for values less than 10,000)
#define PERIOD 1

int main(int argc, char* argv[]) {

    int precision = 13; //13 = default value
    int columns = 3; //3 = default value

    //Overrides default precision and columns with values from a configuration file (if it exist)
    FILE * fp;
    int line1, line2;
    if( (fp = fopen("filter.cfg", "r")) != NULL) {
        fscanf(fp, "%d %d", &line1, &line2);
        precision = line1;
        columns = line2;
        fclose(fp);
    }

    //Overrides default precision and columns with values from the environment variables (if they exist)
    const char *ENV_PRECISION = getenv("CS302HW1PRECISION");
    const char *ENV_COLUMN = getenv("CS302HW1COLUMNS");
    if (ENV_PRECISION != NULL) {
        precision = atoi(ENV_PRECISION);
    }
    if (ENV_COLUMN != NULL) {
        columns = atoi(ENV_COLUMN);
    }

    //Command Line argument for precision format
    if (argc > 2) {
        precision = atoi(argv[1]);
        columns = atoi(argv[2]);
    } else if (argc == 2) {
        precision = atoi(argv[1]);
    }

    //adjustedWidth = width ( + precision + decimal point if precision is > 0)
    const int ADJUSTED_WIDTH = precision > 0 ? WIDTH + PERIOD + precision : WIDTH;

    //process the stream of input numbers
    double streamInput;
    int loopCounter = 1;
    while( scanf(" %lf", &streamInput) != EOF) {
        printf("%*.*f  ", ADJUSTED_WIDTH, precision, streamInput);
        if ( (loopCounter % columns) == 0 ) {
            printf("\n");
        }
        loopCounter++;
    }

    printf("\n");
    return 0;
}
