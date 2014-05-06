#include <stdbool.h>
#include <complex.h>

#include "mandlebrot.h"

/**
 * Check if a point is in the Mandlebrot set.
 */
bool in_mandlebrot(double complex z) {
  double re = creal(z);
  double im = cimag(z);

  for(unsigned int i = 0; i < MAX_ITERATIONS; i++) {
    double re2 = re * re - im * im + creal(z);
    im = 2 * re * im + cimag(z);
    re = re2;

    if(re * re + im * im > 4) {
      return false;
    }
  }

  return true;
}
