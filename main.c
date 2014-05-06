#include <curses.h>
#include <complex.h>
#include <stdbool.h>

#include "mandlebrot.h"

/**
 * Render a fractal to the screen, colouring by how many points per
 * pixel are in the farctal.
 */
static void render_fractal(bool (*in_fractal) (complex double),
                           double complex topleft,
                           double complex bottomright) {
  (void) in_fractal;
  (void) topleft;
  (void) bottomright;
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

  render_fractal(&in_mandlebrot,
                 -5.0 + 5.0 * I,
                 5.0 - 5.0 * I);
  getch();

  curs_set(1);
  nl();
  echo();
  nocbreak();
  delwin(mainwin);
  endwin();
}
