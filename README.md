# Console
C++ display front-end using OpenGL for an experimental windowing system.

# Build Instructions

* Download CMake.
* Check out the repository.
* Run `git submodule init`.
* Run `git submodule update`.
* Run `cmake-gui` and target, in turn, the SDL2 and FreeType2 directories. You will need to pick separate directories to build each in. Use "projectname-bin".
* Build the project.

# TODO

* Figure out why SDL2.dll needs to be present when executable is statically linked.