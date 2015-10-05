#ifndef __MANDELBROT_HEADER__
#define __MANDELBROT_HEADER__

#include <stdint.h>             /* C99 integer types */

/* struct to hold the values for each pixel in the Mandelbrot set calculations.
 * This structure will be used in queries to check that Mandelbrot set values
 * are correct for the multithreaded implementation */
typedef struct index_value
{
  uint32_t i;       /* the row in the image for this pixel */
  uint32_t j;       /* the column in the image for this pixel */
  double value;     /* the magnitude of the (z,zi) distance */
} index_value;

int run_mandelbrot (char *params, index_value ** array);
int run_multibrot (char *params, index_value ** array);

#endif
