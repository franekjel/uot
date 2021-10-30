# Universe of Technology

This repo contains Universe of Technology (UoT) source code. UoT is space 4X game.

## Building

### Linux

Compile like any other cmake project:
```
mkdir build
cd build
cmake .. -GNinja
ninja
```
You can of course omit -GNinja and use make, but ninja is usually faster.

### Windows with Visual Studio 2019

Make sure you have installed cmake and ninja in Visual Studio. Open in VS as cmake project.

### QtCreator

Just open main CMakeList.txt in QtCreator - should works on both Linux and Windows without problems.
