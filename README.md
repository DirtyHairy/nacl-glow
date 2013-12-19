# Glow - a NaCl experiment

## What it is

Glow is a google native client experiment. It displays a canvas on which you can
draw with the mouse; the lines will glow and fade away over time. It
demonstrates the basic structure of a native client application and can be used
as a starting point for other projects.

## How to build it

For building you need to download and install the google NaCl / pepper SDK from
the [webpage](https://developers.google.com/native-client/?hl=de&csw=1). Set the
`NACL_SDK_ROOT` environment variable to the SDK root and type `make` to build
the project; `make clean` and `make realclean` will clean out the generated
files. The build generates x86-32, x86-64 and arm binaries.

You'll have to place the html, the `.nmf` manifest file and the
`.nexe` executables on a webserver in order to try out the program. You can find
a built version [here](http://www.cspeckner.de/nacl/glow). The program builds
fine with pepper version 30.

**Native client currently only works in chrome. If the module fails to load,
make sure that native client is enabled at chrome://flags**

#### PNaCl support

As of Pepper 31, the program works with PNaCl. Call `make pnacl` in order to build
the PNaCl `.pexe` executable. The PNaCl version is available as pnacl.html (hosted
[here](http://www.cspeckner.de/nacl/glow/pnacl.html)). Note that the PNaCl version
might be less stable and will most likely crash on invalid messages from
Javascript as PNaCl currently doesn't support exceptions.

## The controls

* **Radius** Line radius.
* **Bleed** The percentage of intensity distritibuted from a pixel to its eight
  neightbours during each frame. Rounding errors lead to decay over time due to
  bleeding even if the decay controls are set to zero. Zero disables bleeding.
* **Exponential decay** Controls a factor between zero and one which is multiplied
  with each pixels intensity each frame after bleeding. More precisely, it is 15
  minus the "half-time frame count". Zero disables exponential decay.
* **Linear decay** Amount of intensity subtracted each frame after bleeding and
  exponential decay.
* **Target FPS** Frames per second aimed for by the program. Note that the rate
  control algorithm is not very sophisticated, so this will get increasingly
  inaccurate for higher FPS.

In addition, the two FPS displays show the actual measured FPS. *Processing FPS*
are the FPS at which the processing loop runs, while *Rendering FPS* are the FPS
rendered by the browser and which might be lower than the number of frames
processed.
