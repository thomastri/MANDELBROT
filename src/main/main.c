/* ************************************************************************* *
 * main.c                                                                    *
 * ------                                                                    *
 *  Author:   albertar                                                       *
 *  Purpose:  This is a template for project 1.                              *
 * ************************************************************************* */


/* ************************************************************************* *
 * Library includes here.  For documentation of standard C library           *
 * functions, see the list at:                                               *
 *   http://pubs.opengroup.org/onlinepubs/009695399/functions/contents.html  *
 * ************************************************************************* */

#include <stdio.h>              /* standard I/O */
#include <stdbool.h>            /* bool types */
#include <stdint.h>             /* uint32_t, uint8_t, and similar types */
#include <inttypes.h>
#include <stdlib.h>
#include <assert.h>        

#include "parse.h"              /* command line parser */
#include "../mandel/mandel.h"
#include "../msort/msort.h"

bool float_match(float a, float b);

/* main -- main entry point into the program. */
int
main(int argc, char **argv) {

    char *mandel = NULL;
    char *sort = NULL;
    char *query = NULL;
    char *filename = NULL;
    printf("before command\n");
    parse_command_line(argc, argv, &mandel, &sort, &query, &filename);


    if (mandel != NULL && *mandel == 'm') {
        printf("run_mandelbrot()\n");
        run_mandelbrot(filename, NULL);
    }
    else if (sort != NULL && *sort == 's') {
        printf("run run_msort()\n");
        run_msort(filename, NULL);
    }
    else if (query != NULL && *query == 'q') {
        printf("Run query.\n");
        //run_mandelbrot(mandel, array);
        //run_msort(sort, array);
    }
    else {
        return -1;
    }

    return 0;


}

/* checks to see if two floating-point values are within 0.001 of each other */
bool
float_match(float a, float b) {
    if (b > a) return float_match(b, a);

    if (a - b < 0.001) return true;
    else return false;

}

