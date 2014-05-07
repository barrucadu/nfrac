#include <curses.h>
#include <complex.h>
#include <stdbool.h>
#include <string.h>

#include "render.h"
#include "mandlebrot.h"

struct Fractal {
  const char* name;
  double (*renderer) (complex double, const char*[], int);
  int args;
};

/**
 * Define all the fractals available to the program here
 */
static const struct Fractal fractals[] = {
  {"mandlebrot", &in_mandlebrot, 0},
  {"multibrot",  &in_multibrot,  1}};

int main(int argc, const char* argv[]) {
  // Get fractal renderer
  double (*renderer) (complex double, const char *[], int) = &in_mandlebrot;
  const char** frargv = &argv[1];
  int frargc = argc - 1;

  if(frargc > 0) {
    frargc --;

    for(unsigned int i = 0; i < sizeof(fractals) / sizeof(struct Fractal); i++) {
      if(strcmp(fractals[i].name, frargv[0]) == 0) {
        renderer = fractals[i].renderer;
        frargv++;

        if(frargc < fractals[i].args) {
          fprintf(stderr, "'%s' takes %d argument%s.\n",
                  fractals[i].name,
                  fractals[i].args,
                  (fractals[i].args == 1) ? "" : "s");
          return 1;
        }
      }
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

  // Initialise colours
  render_init(mainwin);

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
