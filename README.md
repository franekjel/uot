# Universe of Technology

This repo contains Universe of Technology (UoT) source code. UoT is space 4X game.

## Building

### Linux

Install Qt - tested version is 5.15. Make sure you have qt-3d installed. Then compile like any other cmake project:
```
mkdir build
cd build
cmake .. -GNinja
ninja
```
You can of course omit -Gninja and use make, but ninja is usually faster.

### Windows with Visual Studio 2019

Make sure you have installed cmake and ninja in Visual Studio. Install Qt. Open in VS as cmake project. You may need to configure qt path to build - in project setting or directly in CMakeSettings.json:
```
"cmakeComandArgs": -"DCMAKE_PREFIX_PATH=C:\Qt\\5.15.0\\msvc2019_64"
```
Make sure that this is valid path to your qt installation dir.

### QtCreator

Just open main CMakeList.txt in QtCreator - should works on both Linux and Windows without problems.