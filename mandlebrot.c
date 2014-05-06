#include <stdbool.h>
#include <complex.h>
#include <stddef.h>
#include <stdio.h>

#include "mandlebrot.h"

/**
 * Check if a point is in the Mandlebrot set.
 */
bool in_mandlebrot(double complex z, const char* argv[], int argn) {
  (void) argv;
  (void) argn;

  return in_multibrot(z, NULL, 0);
}

/**
 * Check if a point is in the appropriate Multibrot set.
 * Multibrot is like Mandlebrot, but rather than z^2 we sue z^d, where
 * d is a parameter.
 */
bool in_multibrot(double complex z, const char *argv[], int argn) {
  // Get the parameter, falling back to d=2 if it's not given
  double d;
  if(argn == 0) {
    d = 2.0;
  } else {
    scanf(argv[0], "%lf", &d);
  }

  double complex zi = z;
  double re;
  double im;

  for(unsigned int i = 0; i < MAX_ITERATIONS; i++) {
    zi = cpow(zi, 2) + z;
    re = creal(zi);
    im = cimag(zi);

    if(re * re + im * im > 4) {
      return false;
    }
  }

  return true;
}
