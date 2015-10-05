/* Unithreaded implementation of the Mandelbrot set calculation */

#include <stdio.h>              /* standard I/O */
#include <stdint.h>             /* C99 integer types */
#include <assert.h>             /* declares assert() */
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>
#include "tga.h"                /* TGA image creation */
#include "mandel.h"             /* Mandelbrot header */
#include "pthread.h"
#include <omp.h>                /* OpenMP */

/* Iterate in the Mandelbrot set 500 times (do not change) */
const int NUM_ITERATIONS = 500;

/* Static helper functions. You can change these if needed. */
static void calc_pixel(double x, double y, uint8_t *r, uint8_t *g,
                       uint8_t *b, double *v);

static void calc_mandelbrot(uint8_t *pic, double xstart, double xend,
                            double ystart, double yend, double xstep,
                            double ystep, uint32_t height, uint32_t width);

static void calc_multibroth(uint8_t *pic, double xstart, double xend,
                            double ystart, double yend, double xstep,
                            double ystep, uint32_t height, uint32_t width, uint32_t thread);

static char **parse_file(char *file, char **p);



//pixels explanation
//pixels[row*width*3 + col*3 + color]

/* Entry point to calculate the Mandelbrot set. Do not change the interface */

/*NEW VERSION*/
int
run_mandelbrot(char *params, index_value **_array) {
    // uint8_t *pixels = NULL;
    double xstep = 0;
    double ystep = 0;
    double xleft = 0;
    double yleft = 0;
    double xright = 0;
    double yright = 0;
    char *file_to_write;
    char *arr[8];
    int width, height, thread;

    parse_file(params, arr);

    //extract values from array
    file_to_write = arr[0];
    width = atoi(arr[1]);
    height = atoi(arr[2]);
    thread = atoi(arr[7]);
    xleft = atof(arr[3]);
    yleft = atof(arr[5]);
    xright = atof(arr[4]);
    yright = atof(arr[6]);
    xstep = (xright - xleft) / width;
    ystep = (yright - yleft) / height;

    printf("File %s\n", file_to_write);
    printf("Width is %d\n", width);
    printf("Height is %d\n", height);
    printf("Threads Count %d\n", thread);

    uint8_t *pixels = malloc(8 * height * width * 3);

    if (thread == 0) {
        calc_mandelbrot(pixels, xleft, xright, yleft, yright, xstep, ystep, height, width);
    } else {
        calc_multibroth(pixels, xleft, xright, yleft, yright, xstep, ystep, height, width, thread);
    }

    write_file(file_to_write, pixels, height, width);

    printf("\n\nX Step: %f\n", xstep);
    printf("Y Step: %f\n", ystep);

    return 0;
}

static char **
parse_file(char *file, char **p) {
    printf("In the parse\n");

    FILE *fp = fopen(file, "r");
    char *token;
    char line[100];
    char **array = p;
    int counter = 0;

    if (fp != NULL) {
        printf("reading\n");

        while (fgets(line, 200, fp) != NULL) {
            token = strtok(line, ",");
            while (token != NULL) {
                printf("%s\n", token);
                p[counter] = strdup(token);
                token = strtok(NULL, ",");
                counter++;
            }

        }
        fclose(fp);
    } else {
        perror("File not read");
    }
    return array;

}

/* Unithreaded Mandelbrot calculation */
static void
calc_mandelbrot(uint8_t *pic, double xstart, double xend, double ystart,
                double yend, double xstep, double ystep, uint32_t height,
                uint32_t width) {
    double x, y;
    int i, j;
    uint8_t r, g, b;
    double v;

    assert (pic != NULL);

    /* the main loop */
    x = xstart;
    y = ystart;

    /* for each row in the image */
    for (i = 0; i < height; i++) {
        /* for each column within a row */
        for (j = 0; j < width; j++) {
            /* calculate the RGB pixel values */
            calc_pixel(x, y, &r, &g, &b, &v);
            pic[(i * width + j) * 3 + 0] = b;
            pic[(i * width + j) * 3 + 1] = g;
            pic[(i * width + j) * 3 + 2] = r;

            /* move to the next x value in the row */
            x += xstep;
        }

        /* move to the next y value and start x back at the beginning */
        y += ystep;
        x = xstart;
    }
}

/* Multithreaded Mandelbroth calculation */
static void
calc_multibroth(uint8_t *pic, double xstart, double xend,
                double ystart, double yend, double xstep,
                double ystep, uint32_t height, uint32_t width,
                uint32_t thread_num) {

    int i = 0;
    int rc = 0;
    pthread_t threads[thread_num];
    pthread_attr_t attr;
    void *status = NULL;
    modexp_t params[thread_num];

    //start a parent thread
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);


    //create individual threads
    for (i = 0; i < NUM_THREADS; t++) {
        rc = pthread_create(&threads[i], &attr, mod_exp_thread, (void *) i);
        assert (rc == 0);
    }

    for (i = 0; i < NUM_THREADS; i++) {
        rc = pthread_join(threads[i], &status);

    }

}


void *


/* Calculate the RGB pixel values for a pair of x,y coordinates. */
static void
calc_pixel(double x, double y, uint8_t *red, uint8_t *green, uint8_t *blue,
           double *value) {
    /* z is the real part, zi is the imaginary part of the complex number */
    double z = 0, zi = 0;
    double newz, newzi;    /* used for updating z and zi */
    double color;         /* pixel color */
    bool inset = true;    /* is the point in the Mandelbrot set? */
    int k;

    /* Iterate through the Mandelbrot calculation 500 times. If we ever exceed
     * the threshold, then break out of the loop; we know that point is not in
     * the set. */
    for (k = 0; k < NUM_ITERATIONS; k++) {
        /* z^2 + (x+yi) = (a+bi)(a+bi) + (x+yi) = a^2 - b^2 + x + (2ab + y)i */
        newz = (z * z) - (zi * zi) + x; /* a^2 - b^2 + x */
        newzi = 2 * z * zi + y;       /* (2ab + y)i */
        z = newz;
        zi = newzi;
        if (((z * z) + (zi * zi)) > 100) {
            inset = false;
            color = k;
            k = NUM_ITERATIONS;
        }
    }

    /* evaluate the magnitude of the distance of (z,zi) from (0,0) */
    *value = (z * z) + (zi * zi);

    if (inset) {
        /* points in the set are a blue-ish color */

        *blue = 230;
        *green = 50;
        *red = 70;
    }
    else {
        /* points out of the set are green-ish, lighter as they get closer */

        if (color < 50) color += 50;
        else if (color < 100) color += 55;
        else if (color < 350) color += 80;
        *blue = color / NUM_ITERATIONS * 180;
        *green = color / NUM_ITERATIONS * 255;
        *red = color / NUM_ITERATIONS * 120;
    }

}

/****************************************************************************************************************
 *
 ****************************************************************************************************************/

void calculate_per_row() {
    /* declarations */

    struct info infos[HEIGHT];
    int m;

    //initialize each struct
    for (m = 0; m < HEIGHT; m++) {
        infos[m].x = original.x;
        infos[m].y = original.y;
        infos[m].endx = original.endx;
        infos[m].endy = original.endy;
        infos[m].incrementX = original.x;
        infos[m].incrementY = original.incrementY;
    }

    long i, t;
    void *status;

    pthread_t threads[HEIGHT];
    pthread_attr_t attr;
    int rc;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (i = 0; i < HEIGHT; i++) {
        rc = pthread_create(&threads[i], &attr, width_and_color, (void *) &infos[i]);
        infos[i].height++;
    }

    pthread_attr_destroy(&attr);
    for (t = 0; t < HEIGHT; t++) {
        rc = pthread_join(threads[t], &status);
        if (rc) {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
        printf("Main: completed join with thread %ld having a status of %ld\n", t, (long) status);
    }
    write_file(outputName, (uint8_t *) pixels);
    pthread_exit(NULL);
}

/**************************************
 *
 **************************************/

void *width_and_color(void *ii) {
    struct info *reference = (struct info *) ii;

    int j;
    struct colorpixels cp;

    for (j = 0; j < WIDTH; j++) {
        find_color(&cp, &reference);
        pixels[reference->height][j][0] = cp.blue;
        pixels[reference->height][j][1] = cp.green;
        pixels[reference->height][j][2] = cp.red;
        reference->x += reference->incrementX;
    }

    reference->y += reference->incrementY;
    reference->x = reference->firstX;
    return 0;
}

void *find_color(void *cp2, void *rf) {
    struct colorpixels *cp;
    struct info *r;
    cp = (struct colorpixels *) cp2;
    r = (struct info *) rf;

    int k;
    uint8_t blue, green, red;
    double a, b, newa, newb, color;

    a = 0;
    b = 0;
    color = 0;

    for (k = 0; a * a + b * b < 100 && k < NUM_ITERATIONS; k++) //magnitude of the orbit point
    {
        newa = a * a - b * b + r->x;
        newb = 2 * a * b + r->y;
        a = newa;
        b = newb;
    }

    if (a * a + b * b < 100) {
        blue = 230;
        green = 50;
        red = 70;
    }
    else {
        if (k < 50) color = k + 50;
        else if (k < 100) color = k + 55;
        else if (k < 350) color = k + 80;
        else if (k < 400) color = k + 90;
        else color = k + (499 - k);

        blue = (color / (double) NUM_ITERATIONS * 180);
        green = (color / (double) NUM_ITERATIONS * 255);
        red = (color / (double) NUM_ITERATIONS * 120);
    }

    cp->blue = blue;
    cp->green = green;
    cp->red = red;

    return 0;
}