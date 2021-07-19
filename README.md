# Universe of Technology

This repo contains Universe of Technology (UoT) source code. UoT is space 4X game.

## Building

### Linux

Download Qt - tested version is 5.15. Make sure you have qt-3d installed. Then compile like any other cmake project:
```
mkdir build
cd build
cmake .. -GNinja
ninja
```
You can of course omit -Gninja and use make, but ninja is usually faster.

### Windows with Visual Studio

TODO (short: install Qt and cmake in Visual Studio, usually works)
