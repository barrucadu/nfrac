#include <curses.h>
#include <complex.h>
#include <stdbool.h>

#include "mandlebrot.h"

/**
 * Size of square area to render in.
 */
int screen_size;

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

  // Get terminal size
  int max_y, max_x;
  getmaxyx(mainwin, max_y, max_x);

  // Set size of rendering area
  screen_size = (max_y < max_x) ? max_y : max_x;

  // Render a fractal
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
