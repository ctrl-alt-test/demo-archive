This directory contains the source code of F - Felix's Workshop (released in
April 2012). For more information about Felix's Workshop, including a video
capture, visit our website:
http://www.ctrl-alt-test.fr/productions/f-felixs-workshop/

Too many comments were written in French. Sorry about that. Some of the code is
uselessly complex (did you know the intro includes a physics engine?).

Most of the code was written by Julien Guertault and Laurent Le Brun. Some
exceptions:

* kkrunchy_023a4_asm07.exe comes from http://www.farbrausch.de/~fg/kkrunchy/
* picoc comes from https://github.com/zsaleeba/picoc
* sed.exe comes from http://gnuwin32.sourceforge.net/packages/sed.htm
* libv2.lib comes from http://www.pouet.net/prod.php?which=15073
* tweakval comes from https://www.gamedev.net/resources/_/technical/game-programming/tweakable-constants-r2731
* some files (mostly in src/sys) come from http://iquilezles.org/code/index.htm

## Compiling & Testing

The code was originally built using Visual C++ Express 2008. It has been later
updated to work with Visual Studio Community 2019. It can be run in Debug
mode. If you manage to build it in Release mode, please send a pull request.

When you run it in Debug mode, it is interactive and you may use your
keyboard/mouse. Try:

 - `S` to enable the music
 - `P` for the pause
 - `Y`, `U`, `I`, `O` to move the time backward/forward
 - `H` to use the manual camera (use arrow keys or the mouse).
 - `G` to move the manual camera back to the default position
 - `W`, `X` to control the field of view
 - `B`, `N` to control the depth of field
 - `A`, `Q` to view the textures

When moving in the scene with the mouse, use left click to rotate, right click
to translate. Use both buttons to rotate the view.
