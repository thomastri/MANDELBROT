/* command line parsing routine */

#include <stdio.h>              /* standard I/O */
#include <stdlib.h>
#include <unistd.h>             /* declares getopt() */
#include <stdlib.h>             /* declares exit() */
#include <getopt.h>             /* header for getopt */
#include "parse.h"              /* prototypes for exported functions */
#include <unistd.h>

/* Show the help message that explains command-line options */
void
show_usage(void) {
    printf("Usage: mandel [options]\n");
    printf("Options:\n");
    printf("  -m F         Run Mandelbrot with parameters in file F\n");
    printf("  -s F         Sort the data set with parameters in file F\n");
    printf("  -q F         Query the data set with parameters in file F\n");
    printf("  -h           Print this message and exit\n");
}

/* parse_command_line -- parses the command-line arguments using getopt()    */
int parse_command_line(int argc, char **argv, char **mandel_params,
                       char **sort_params, char **query_params, char **file) {
    int option;
    int mflag = 0, sflag = 0, qflag = 0;


    while ((option = getopt(argc, argv, "hm:s:q:")) != -1) {
        switch (option) {
            case 'h':
                show_usage();
                printf("Print this message and exit\n");
                break;
            case 'm': // Call mandelbrot here
                mflag++;
                *file = optarg;
                printf("Input file: \"%s\"\n", optarg);
                printf("Run Mandelbrot with parameters in file F\n");
                break;
            case 's': // Call mergesort here
                sflag++;
                *file = optarg;
                printf("Sort the data set with parameters in file F\n");
                break;
            case 'q': // FULL IMPLEMENTATION. Call both mandelbrot and msort here, and write to query
                qflag++;
                *file = optarg;
                printf("Query the data set with parameters in file F\n");
                break;
            default:
                return 1;
        }

        //Check for valid file.

        if (mflag == 1) {
            *mandel_params = "m";
        }
        else if (sflag == 1) {
            *sort_params = "s";
        }
        else if (qflag == 1) {
            *query_params = "q";
        }

    }
    return 0;
}
