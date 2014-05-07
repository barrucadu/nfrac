#ifndef __RENDER_H
#define __RENDER_H

#include <curses.h>
#include <complex.h>

/**
 * Number of complex points to include per row/col
 */
#define RESOLUTION 5

/**
 * Rendering colour pair names.
 */
#define NONE 0
#define FEW  1
#define SOME 2
#define MANY 3
#define LOTS 4
#define ALL  5
#define BG   6

/**
 * Initialise colours and get screen size
 */
void render_init(WINDOW* mainwin);

/**
 * Zoom in one step
 */
void zoom_in(void);

/**
 * Zoom out one step
 */
void zoom_out(void);

/**
 * Scroll some number of pixels
 */
void scrollpixels(int y, int x);

/**
 * Center the image on a point
 */
void centre(double complex z);

/**
 * Reset the rendering to the starting position
 */
void reset(void);

/**
 * Toggle background hiding
 */
void toggle_hide(void);

/**
 * Toggle bright colours
 */
void toggle_bright(void);

/**
 * Helper function: get the complex coordinates of the top-left of the
 * given pixel
 */
double complex pixel_topleft(int y, int x);

/**
 * Render a fractal to the screen, colouring by how many points per
 * pixel are in the fractal.
 */
void render_fractal(double (*in_fractal) (complex double, const char *[], int),
                    const char * frargv[], int frargn);

#endif
