#include <curses.h>
#include <complex.h>
#include <stdbool.h>
#include <math.h>

#include "mandlebrot.h"

/**
 * Number of complex points to include per row/col
 */
#define RESOLUTION 100

/**
 * Rendering colour pairs
 */
#define NONE 0
#define FEW  1
#define SOME 2
#define MANY 3
#define LOTS 4
#define ALL  5

/**
 * Size of square area to render in.
 */
unsigned int screen_size;

/**
 * Render a fractal to the screen, colouring by how many points per
 * pixel are in the fractal.
 */
static void render_fractal(bool (*in_fractal) (complex double),
                           double complex topleft,
                           double complex bottomright) {
  // Get the size of the grid
  double range_re = creal(bottomright) - creal(topleft);
  double range_im = cimag(bottomright) - cimag(topleft);

  // Figure out how many complex numbers per Y/X point we have
  unsigned int perpoint = (unsigned int)((fabs(range_re) * RESOLUTION) / screen_size);

  // Calculate the rendering step
  double step = 1.0 / perpoint;

  //printf("%f, %f, %i, %f\n", range_re, range_im, perpoint, step);

  // Compute the fractal
  for(unsigned int y = 0; y < screen_size; y ++) {
    for(unsigned int x = 0; x < screen_size; x ++) {
      // Compute how many points for this "pixel" are in the fractal.
      unsigned int in = 0;
      double re_off = range_re * ((double)x / (double)screen_size);
      double im_off = range_im * ((double)y / (double)screen_size);
      double complex base = topleft + re_off + im_off * I;

      for(unsigned int py = 0; py < perpoint; py ++) {
        for(unsigned int px = 0; px < perpoint; px ++) {
          double complex point = px * step + py * step * I;
          if(in_fractal(base + point))
            in ++;
        }
      }

      // Get the fraction of points which are in the fractal
      double in_frac = (float)in / (float)(perpoint * perpoint);

      // Select the colour
      unsigned int cpair = ALL;
      if(in_frac == 0) {
        cpair = NONE;
      } else if(in_frac <= 0.2) {
        cpair = FEW;
      } else if(in_frac <= 0.4) {
        cpair = SOME;
      } else if(in_frac <= 0.6) {
        cpair = MANY;
      } else if(in_frac <= 0.8) {
        cpair = LOTS;
      }

      // Select the char
      char render = (in_frac < 0.5) ? '.' : '#';

      // Render the point
      attron(COLOR_PAIR(cpair));
      mvaddch(y, x, render);
      //mvaddch(y, x, '0' + in);
      attroff(COLOR_PAIR(cpair));
      //printf("(%f, %f) ", re_off, im_off);
    }
    //printf("\n");
  }
}

int main() {
  WINDOW * mainwin = initscr();
  start_color();
  cbreak();
  noecho();
  nonl();
  intrflush(stdscr, false);
  keypad(stdscr, true);
  curs_set(0);

  // Get terminal size
  int max_y, max_x;
  getmaxyx(mainwin, max_y, max_x);

  // Set size of rendering area
  screen_size = (unsigned int) ((max_y < max_x) ? max_y : max_x);

  // Initialise colours
  init_pair(NONE, COLOR_WHITE,   COLOR_BLACK);
  init_pair(FEW,  COLOR_CYAN,    COLOR_BLACK);
  init_pair(SOME, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(MANY, COLOR_BLUE,    COLOR_BLACK);
  init_pair(LOTS, COLOR_YELLOW,  COLOR_BLACK);
  init_pair(ALL,  COLOR_GREEN,   COLOR_BLACK);

  // Render a fractal
  render_fractal(&in_mandlebrot,
                 -2.0 + 2.0 * I,
                 2.0 - 2.0 * I);
  getch();

  curs_set(1);
  nl();
  echo();
  nocbreak();
  delwin(mainwin);
  endwin();
}
