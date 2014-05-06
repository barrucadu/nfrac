#include <curses.h>
#include <complex.h>
#include <stdbool.h>
#include <math.h>

#include "mandlebrot.h"

/**
 * Number of complex points to include per row/col
 */
#define RESOLUTION 5

/**
 * Rendering colour pairs
 */
#define NONE 0
#define FEW  1
#define SOME 2
#define MANY 3
#define LOTS 4
#define ALL  5

#define SELECTED 6

/**
 * Size of area to render in.
 */
unsigned int screen_height;
unsigned int screen_width;

/**
 * Helper function: get the complex coordinates of the top-left of the
 * given pixel
 */
static double complex pixel_topleft(double complex topleft,
                                    double complex bottomright,
                                    int y, int x) {
  // Get the size of the grid
  double range_re = creal(bottomright) - creal(topleft);
  double range_im = cimag(bottomright) - cimag(topleft);

  // Get the re and im offsets for the position
  double re_off = range_re * ((double)x / (double)screen_width);
  double im_off = range_im * ((double)y / (double)screen_height);

  // Return the position
  return topleft + re_off + im_off * I;
}

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

  // Calculate the rendering step
  double stepx = (range_re / screen_width) / RESOLUTION;
  double stepy = (range_im / screen_height) / RESOLUTION;

  // Compute the fractal
  for(unsigned int y = 0; y < screen_height; y ++) {
    for(unsigned int x = 0; x < screen_width; x ++) {
      // Compute how many points for this "pixel" are in the fractal.
      unsigned int in = 0;

      // Get the corner of this pixel
      double complex base = pixel_topleft(topleft, bottomright, (int)y, (int)x);

      for(unsigned int py = 0; py <= RESOLUTION; py ++) {
        for(unsigned int px = 0; px <= RESOLUTION; px ++) {
          double complex point = px * stepx + py * stepy * I;
          if(in_fractal(base + point))
            in ++;
        }
      }

      // Get the fraction of points which are in the fractal
      double in_frac = (double)in / (double)(RESOLUTION * RESOLUTION);

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
      attroff(COLOR_PAIR(cpair));
    }
  }
}

int main() {
  MEVENT event;
  WINDOW * mainwin = initscr();
  start_color();
  cbreak();
  noecho();
  nonl();
  intrflush(stdscr, false);
  keypad(stdscr, true);
  curs_set(0);
  mousemask(ALL_MOUSE_EVENTS, NULL);

  // Get terminal size
  int max_y, max_x;
  getmaxyx(mainwin, max_y, max_x);

  // Set size of rendering area
  screen_height = (unsigned int)max_y;
  screen_width = (unsigned int)max_x;

  // Initialise colours
  init_pair(NONE, COLOR_WHITE,   COLOR_BLACK);
  init_pair(FEW,  COLOR_CYAN,    COLOR_BLACK);
  init_pair(SOME, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(MANY, COLOR_BLUE,    COLOR_BLACK);
  init_pair(LOTS, COLOR_YELLOW,  COLOR_BLACK);
  init_pair(ALL,  COLOR_GREEN,   COLOR_BLACK);

  init_pair(SELECTED, COLOR_RED, COLOR_BLACK);

  // Render the fractal
  double complex topleft = -2.0 + 2.0 * I;
  double complex bottomright = 2.0 - 2.0 * I;

  // Store mouse selection coordinates
  bool selected = false;
  int sely, selx;

  bool looping = true;
  while(looping) {
    // Render the fractal
    render_fractal(&in_mandlebrot, topleft, bottomright);

    // Render a selection
    if(selected) {
      attron(COLOR_PAIR(SELECTED));
      attron(A_BOLD);
      mvaddch(sely, selx, '+');
      attroff(A_BOLD);
      attroff(COLOR_PAIR(SELECTED));
    }

    int ch = getch();
    switch(ch) {
    case KEY_MOUSE:
      if(getmouse(&event) == OK) {
        if(event.bstate & BUTTON1_CLICKED) {
          if(selected) {
            if(event.x == selx && event.y == sely) {
              // Unselect by clicking the selected point
              selected = false;
            } else {
              // Otherwise zoom!
              double complex newtl = pixel_topleft(topleft, bottomright,
                                                   sely, selx);
              double complex newbr = pixel_topleft(topleft, bottomright,
                                                   event.y, event.x);
              topleft = newtl;
              bottomright = newbr;
              selected = false;
            }
          } else {
            // Otherwise select a point
            selx = event.x;
            sely = event.y;
            selected = true;
          }
        }
      }
      break;

    case 'q':
      looping = false;
      break;

    default:
      break;
    }
  }

  curs_set(1);
  nl();
  echo();
  nocbreak();
  delwin(mainwin);
  endwin();
}
