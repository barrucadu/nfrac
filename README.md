nfrac: a fractal viewer
=======================

nfrac is a little fractal viewer written in C and using
ncurses. Currently the only fractal it supports is the Mandlebrot set,
but adding more (assuming they don't require user input) shouldn't be
tricky.

Invocation
----------

    ./nfrac [fractal] [fractal args...]

Current supported fractals are:

 - mandlebrot: no arguments
 - multibrot: 1 float argument

If no fractal is given, Mandlebrot is assumed.

Controls
--------

 - arrows: move display
 - +/-: zoom in/out
 - click: centre on the selected point
 - h: toggle background dots
 - b: toggle bright colours
 - r: reset to initial view
 - q: quit

Defining Fractals
-----------------

Fractals are defined by writing a function (see in_multibrot in
mandlebrot.c for an example) which determines how close to the fractal
the provided point is, and adding the fractal to the list in main.c

The value returned by the fractal function is a distance metric,
implemented however you want, where 0 indicates definitely in the
fractal (within reasonable computational limits), and any other value
indicates not int he fractal. How close the value is to 1 determines
how close the point is to being in the fractal, and is used in
colouring.
