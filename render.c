#include <complex.h>
#include <ncurses.h>
#include <math.h>
#include <stdlib.h>

#include "render.h"

/**
 * Colour pair numbers
 */
short colours_dark[7] = {0, 1, 2, 3, 4, 5, 6};
short colours_bright[7] = {7, 8, 9, 10, 11, 12, 13};
static short *colours = &colours_dark[0];

/**
 * Size of area to render in.
 */
unsigned int screen_height;
unsigned int screen_width;

/**
 * Corners of screen
 */
static double complex topleft = -5.0 + 5.0 * I;
static double complex bottomright = 5.0 - 5.0 * I;

/**
 * Whether to hide the background or not
 */
static bool hide = true;

/**
 * Whether to use bright colours or not
 */
static bool bright = false;

/**
 * The curses window we render to
 */
WINDOW *win;

/**
 * Initialise colours and get screen size
 */
void render_init(WINDOW *mainwin) {
  // Save window
  win = mainwin;

  // Get rendering area size
  getmaxyx(mainwin, screen_height, screen_width);
  screen_height --;

  // Initialise colours
  init_pair(colours_dark[NONE], COLOR_WHITE,   COLOR_BLACK);
  init_pair(colours_dark[FEW],  COLOR_CYAN,    COLOR_BLACK);
  init_pair(colours_dark[SOME], COLOR_MAGENTA, COLOR_BLACK);
  init_pair(colours_dark[MANY], COLOR_BLUE,    COLOR_BLACK);
  init_pair(colours_dark[LOTS], COLOR_YELLOW,  COLOR_BLACK);
  init_pair(colours_dark[ALL],  COLOR_GREEN,   COLOR_BLACK);
  init_pair(colours_dark[BG],   COLOR_BLACK,   COLOR_BLACK);

  init_pair(colours_bright[NONE], COLOR_BLACK,   COLOR_WHITE);
  init_pair(colours_bright[FEW],  COLOR_MAGENTA, COLOR_WHITE);
  init_pair(colours_bright[SOME], COLOR_RED,     COLOR_WHITE);
  init_pair(colours_bright[MANY], COLOR_YELLOW,  COLOR_WHITE);
  init_pair(colours_bright[LOTS], COLOR_BLUE,    COLOR_WHITE);
  init_pair(colours_bright[ALL],  COLOR_CYAN,    COLOR_WHITE);
  init_pair(colours_bright[BG],   COLOR_RED,     COLOR_WHITE);

}

/**
 * Zoom in one step
 */
void zoom_in() {
  double complex offset = (creal(bottomright) - creal(topleft)) / 4;
  topleft += offset;
  bottomright -= offset;

  offset = (cimag(bottomright) - cimag(topleft)) / 4;
  topleft += offset * I;
  bottomright -= offset * I;
}

/**
 * Zoom out one step
 */
void zoom_out() {
  double complex offset = (creal(bottomright) - creal(topleft)) / 4;
  topleft -= offset;
  bottomright += offset;

  offset = (cimag(bottomright) - cimag(topleft)) / 4;
  topleft -= offset * I;
  bottomright += offset * I;
}

/**
 * Scroll some number of pixels
 */
void scrollpixels(int y, int x) {
  double complex offset = pixel_topleft(0, 0) - pixel_topleft(abs(y), 0);
  if(y < 0)
    offset = -offset;
  topleft += offset;
  bottomright += offset;

  offset = pixel_topleft(0, 0) - pixel_topleft(0, abs(x));
  if(x < 0)
    offset = -offset;
  topleft += offset;
  bottomright += offset;
}

/**
 * Center the image on a point
 */
void centre(double complex z) {
  double range_re = creal(bottomright) - creal(topleft);
  double range_im = cimag(bottomright) - cimag(topleft);
  topleft = z - (range_re / 2.0 + range_im / 2.0 * I);
  bottomright = z + range_re / 2.0 + range_im / 2.0 * I;  
}

/**
 * Reset the rendering to the starting position
 */
void reset() {
  topleft = -5.0 + 5.0 * I;
  bottomright = 5.0 - 5.0 * I;
}

/**
 * Toggle background hiding
 */
void toggle_hide() {
  hide = !hide;
}

/**
 * Toggle bright colours
 */
void toggle_bright() {
  bright = !bright;
  colours = bright ? &colours_bright[0] : &colours_dark[0];
}

/**
 * Helper function: get the complex coordinates of the top-left of the
 * given pixel
 */
double complex pixel_topleft(int y, int x) {
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
void render_fractal(double (*in_fractal) (complex double, const char *[], int),
                    const char * frargv[], int frargn) {
  clear();

  // Get the size of the grid
  double range_re = creal(bottomright) - creal(topleft);
  double range_im = cimag(bottomright) - cimag(topleft);

  // Calculate the rendering step
  double stepx = (range_re / screen_width) / RESOLUTION;
  double stepy = (range_im / screen_height) / RESOLUTION;

  // Compute the fractal
  for(unsigned int y = 0; y < screen_height; y ++) {
    for(unsigned int x = 0; x < screen_width; x ++) {
      // The sum of distances
      double distsum = 0.0;

      // The number of points in the fractal
      unsigned int in = 0;

      // Get the corner of this pixel
      double complex base = pixel_topleft((int)y, (int)x);

      for(unsigned int py = 0; py <= RESOLUTION; py ++) {
        for(unsigned int px = 0; px <= RESOLUTION; px ++) {
          double complex point = px * stepx + py * stepy * I;
          double dist = in_fractal(base + point, frargv, frargn);
          distsum += dist;
          if(dist == 1)
            in ++;
        }
      }

      // Get the average distance
      double distance = distsum / (double)(RESOLUTION * RESOLUTION);

      // Get the proportion in the fractal
      double in_frac = (double)in / (double)(RESOLUTION * RESOLUTION);

      // Get the colour metric
      double colour_metric = (2.0 * distance + in_frac) / 3.0;

      // Select the colour
      short cpair = colours[ALL];
      if(colour_metric <= 0.1) {
        cpair = colours[NONE];
      } else if(colour_metric <= 0.2) {
        cpair = colours[FEW];
      } else if(colour_metric <= 0.4) {
        cpair = colours[SOME];
      } else if(colour_metric <= 0.6) {
        cpair = colours[MANY];
      } else if(colour_metric <= 0.8) {
        cpair = colours[LOTS];
      }

      // Select the char
      char render = (in_frac < 0.5) ? '.' : '#';

      // Hide background
      if(cpair == colours[NONE] && hide) {
        cpair = colours[BG];
        render = ' ';
      }

      // Render the point
      attron(COLOR_PAIR(cpair));
      mvwaddch(win, y, x, render);
      attroff(COLOR_PAIR(cpair));
    }
  }

  // Display scale info
  char buf[screen_width];
  snprintf(buf, screen_width, "From (%f + %fi) to (%f + %fi)%s%s",
           creal(topleft), cimag(topleft),
           creal(bottomright), cimag(bottomright),
           hide ? " [h]" : "",
           bright ? " [b]" : "");
  move(screen_height, 0);
  clrtoeol();
  mvwaddstr(win, screen_height, 0, buf);
}
