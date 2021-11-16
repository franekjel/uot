# Universe of Technology

This repo contains Universe of Technology (UoT) source code. UoT is space 4X game.

## Building

### Linux
Needed dependencies are SDL2, SDL2-image, SDL2-ttf, protobuf and libprotobuf (development files for all).

Compile like any other cmake project:
```
mkdir build
cd build
cmake .. -GNinja
ninja
```

### Windows with Visual Studio 2019

Make sure you have installed cmake and ninja in Visual Studio.

On windows dependencies may by installed by hand (same as for linux, but harder) or using vcpkg.

1. Download vcpkg:
```
git clone https://github.com/microsoft/vcpkg
```
2. Install bootstrap - in downloaded vcpkg folder run ```bootstrap-vcpkg.bat```
3. Install required dependencies:
```
vcpkg install sdl2 sdl2-image sdl2-ttf gamenetworkingsockets --triplet x64-windows
```
4. Install visual integration (require admin):
```
vcpkg integrate install
```

Open in VS as cmake project. There are some problems with debug builds - for some reasons they request normal libraries not *d.dll ones, but release builds works fine.

