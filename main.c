#include <curses.h>
#include <complex.h>
#include <stdbool.h>
#include <string.h>

#include "render.h"
#include "mandlebrot.h"

extern short colours_dark[7];
extern short colours_bright[7];
extern unsigned int screen_width;
extern unsigned int screen_height;

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
  init_pair(colours_bright[BG],   COLOR_RED,     COLOR_WHITE);

  bool looping = true;
  while(looping) {
    // Render the fractal
    render_fractal(renderer, frargv, frargc);

    // Get input
    int ch = getch();
    switch(ch) {
    case KEY_UP:
      scrollpixels(1, 0);
      break;

    case KEY_DOWN:
      scrollpixels(-1, 0);
      break;

    case KEY_LEFT:
      scrollpixels(0, 1);
      break;

    case KEY_RIGHT:
      scrollpixels(0, -1);
      break;

    case '+':
      zoom_in();
      break;

    case '-':
      zoom_out();
      break;

    case 'h':
      toggle_hide();
      break;

    case 'b':
      toggle_bright();
      break;

    case KEY_MOUSE:
      if(getmouse(&event) == OK) {
        if(event.bstate & BUTTON1_CLICKED) {
          centre(pixel_topleft(event.y, event.x));
        }
      }
      break;

    case 'r':
      reset();
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
