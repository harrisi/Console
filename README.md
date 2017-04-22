# Console
C++ display front-end using OpenGL for an experimental windowing system.

# Build Instructions

* Download CMake.
* Check out the repository.
* Follow the platform specific instructions.

## GNU/Linux

* Install the dependencies via your package manager.
* Run `cmake .`.
* Run `make`.
* If there are errors finding FreeType2's includes, run: `g++ $(sdl2-config --cflags --libs) $(freetype-config --cflags --libs) --std=c++11 console.cc -o console`.

## Windows

* Edit the test for FreeType out of CMakeLists.txt.
* `cmake . -G "Visual Studio 15 2017 Win64" -D"SDL2_INCLUDE_DIRS=SDL2-2.0.5\include" -D"SDL2_LIBRARY=SDL2-2.0.5-bin\Debug\SDL2;SDL2-2.0.5-bin\Debug\SDL2main" -D"FREETYPE_INCLUDE_DIRS=freetype2\include" -D"FREETYPE_LIBRARIES=freetype2-bin\Debug\freetyped"`.
* Build the solution.
* Copy SDL2.dll into the output directory.

## MacOS

Compiling, using brew as prereq gatherer:

* `brew install sdl2 freetype`
* `g++ $(sdl2-config --cflags --libs) $(freetype-config --cflags --libs)
  -framework OpenGL --std=c++11 console.cc -o console`

# TODO

* Figure out why SDL2.dll needs to be present when executable is statically linked.
