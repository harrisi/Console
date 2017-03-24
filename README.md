# Console
C++ display front-end using OpenGL for an experimental windowing system.

# Build Instructions

* Download CMake.
* Check out the repository.
* Run `git submodule init`.
* Run `cmake-gui` and target, in turn, the SDL2 and FreeType2 directories. You will need to pick separate directories to build each in. I chose "projectname-bin".
* To reduce project clutter I copied the binary output directories into the source directories and deleted the build directories. For both projects this is the "Debug" directory at the top level.
* Open the solution and in the solution explorer pane right click on the Console project and select Properties. You will need to change the Include and Library locations under the VC++ Directories pane as Visual Studio uses absolute paths.
* Build the project.