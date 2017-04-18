# Console
C++ display front-end using OpenGL for an experimental windowing system.

# Build Instructions

* Download CMake.
* Check out the repository.
* Run `git submodule init`.
* Run `git submodule update`.
* Run `cmake-gui` and target, in turn, the SDL2 and FreeType2 directories. You will need to pick separate directories to build each in. Use "projectname-bin".
* Build the project.

# Windows

* Edit the test for FreeType out of CMakeLists.txt.
* `cmake . -G "Visual Studio 15 2017 Win64" -D"SDL2_INCLUDE_DIRS=SDL2-2.0.5\include" -D"SDL2_LIBRARY=SDL2-2.0.5-bin\Debug\SDL2;SDL2-2.0.5-bin\Debug\SDL2main" -D"FREETYPE_INCLUDE_DIRS=freetype2\include" -D"FREETYPE_LIBRARIES=freetype2-bin\Debug\freetyped"`.
* Build the solution.
* Copy SDL2.dll into the output directory.

# MacOS

Compiling, using brew as prereq gatherer:

* `brew install sdl2 freetype`
* `g++ -I /usr/local/Cellar/sdl2/2.0.5/include/ -I
/usr/local/Cellar/freetype/2.7.1/include/ -I
/usr/local/Cellar/freetype/2.7.1/include/freetype2/ -L /usr/local/lib -lSDL2
-lfreetype -framework OpenGL Console.cc -o Console`

# TODO

* Figure out why SDL2.dll needs to be present when executable is statically linked.