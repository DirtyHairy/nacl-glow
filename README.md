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
`.pexe` executables on a webserver in order to try out the program. You can find
a built version [here]('http://www.cspeckner.de/nacl/glow'). The program builds
fine with pepper version 30.
