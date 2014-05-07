#ifndef __MANDLEBROT_H
#define __MANDLEBROT_H

#include <complex.h>

/**
 * Maximum number of iterations to use when computing if a point is in or not
 */
#define MAX_ITERATIONS 100

/**
 * Check if a point is in the Mandlebrot set.
 */
double in_mandlebrot(double complex z, const char* argv[], int argn);
double in_multibrot(double complex z, const char *argv[], int argn);

#endif
