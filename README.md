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
