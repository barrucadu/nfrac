#include <curses.h>
#include <complex.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include "mandlebrot.h"

/**
 * Number of complex points to include per row/col
 */
#define RESOLUTION 5

/**
 * Rendering colour pairs.
 */
#define NONE 0
#define FEW  1
#define SOME 2
#define MANY 3
#define LOTS 4
#define ALL  5
#define BG   6

static short colours_dark[7] = {0, 1, 2, 3, 4, 5, 6};
static short colours_bright[7] = {7, 8, 9, 10, 11, 12, 13};
static short *colours = &colours_dark[0];

/**
 * Size of area to render in.
 */
unsigned int screen_height;
unsigned int screen_width;

/**
 * Corners of screen
 */
double complex topleft = -5.0 + 5.0 * I;
double complex bottomright = 5.0 - 5.0 * I;

/**
 * Whether to hide the background or not
 */
bool hide = true;

/**
 * Whether to use bright colours or not
 */
bool bright = false;

/**
 * Helper function: get the complex coordinates of the top-left of the
 * given pixel
 */
static double complex pixel_topleft(int y, int x) {
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
static void render_fractal(double (*in_fractal) (complex double, const char *[], int),
                           const char * frargv[], int frargn) {
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

      // Render the point
      if(cpair == colours[NONE] && hide) {
        attron(COLOR_PAIR(colours[BG]));
        mvaddch(y, x, ' ');
        attroff(COLOR_PAIR(colours[BG]));
      } else {
        attron(COLOR_PAIR(cpair));
        mvaddch(y, x, render);
        attroff(COLOR_PAIR(cpair));
      }
    }
  }
}

int main(int argc, const char* argv[]) {
  // Get fractal renderer
  double (*renderer) (complex double, const char *[], int) = &in_mandlebrot;
  const char** frargv = &argv[1];
  int frargc = argc - 1;

  if(frargc > 0) {
    frargc --;

    if(strcmp("mandlebrot", frargv[0]) == 0) {
      renderer = &in_mandlebrot;
      frargv ++;
    } else if(strcmp("multibrot", frargv[0]) == 0) {
      renderer = &in_multibrot;
      frargv ++;

      if(frargc == 0) {
        fprintf(stderr, "'multibrot' takes an argument.\n");
        return 2;
      }
    } else {
      fprintf(stderr, "Unknown fractal: %s\n", frargv[0]);
      return 1;
    }
  }

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
  screen_height = (unsigned int)max_y - 1;
  screen_width = (unsigned int)max_x;

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
  init_pair(colours_bright[BG],   COLOR_RED,   COLOR_WHITE);

  bool looping = true;
  while(looping) {
    clear();

    // Render the fractal
    render_fractal(renderer, frargv, frargc);

    // Display scale info
    char buf[screen_width];
    snprintf(buf, screen_width, "From (%f + %fi) to (%f + %fi)%s%s",
             creal(topleft), cimag(topleft),
             creal(bottomright), cimag(bottomright),
             hide ? " [h]" : "",
             bright ? " [b]" : "");
    move(screen_height, 0);
    clrtoeol();
    mvaddstr(screen_height, 0, buf);

    double complex offset;

    int ch = getch();
    switch(ch) {
    case KEY_UP:
      offset = pixel_topleft(0, 0) - pixel_topleft(1, 0);
      topleft += offset;
      bottomright += offset;
      break;

    case KEY_DOWN:
      offset = -(pixel_topleft(0, 0) - pixel_topleft(1, 0));
      topleft += offset;
      bottomright += offset;
      break;

    case KEY_LEFT:
      offset = pixel_topleft(0, 0) - pixel_topleft(0, 1);
      topleft += offset;
      bottomright += offset;
      break;

    case KEY_RIGHT:
      offset = -(pixel_topleft(0, 0) - pixel_topleft(0, 1));
      topleft += offset;
      bottomright += offset;
      break;

    case '+':
      offset = (creal(bottomright) - creal(topleft)) / 4;
      topleft += offset;
      bottomright -= offset;
      offset = (cimag(bottomright) - cimag(topleft)) / 4;
      topleft += offset * I;
      bottomright -= offset * I;
      break;

    case '-':
      offset = (creal(bottomright) - creal(topleft)) / 4;
      topleft -= offset;
      bottomright += offset;
      offset = (cimag(bottomright) - cimag(topleft)) / 4;
      topleft -= offset * I;
      bottomright += offset * I;
      break;

    case 'h':
      hide = !hide;
      break;

    case 'b':
      bright = !bright;
      colours = bright ? &colours_bright[0] : &colours_dark[0];
      break;

    case KEY_MOUSE:
      if(getmouse(&event) == OK) {
        if(event.bstate & BUTTON1_CLICKED) {
          double range_re = creal(bottomright) - creal(topleft);
          double range_im = cimag(bottomright) - cimag(topleft);
          double complex centre = pixel_topleft(event.y, event.x);
          topleft = centre - (range_re / 2.0 + range_im / 2.0 * I);
          bottomright = centre + range_re / 2.0 + range_im / 2.0 * I;
        }
      }
      break;

    case 'r':
      topleft = -5.0 + 5.0 * I;
      bottomright = 5.0 - 5.0 * I;
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
