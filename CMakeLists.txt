cmake_minimum_required(VERSION 2.8.9)
project (Console)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "./CMake")

find_package (OpenGL REQUIRED)
include_directories (${OPENGL_INCLUDE_DIRS})

# The below needs to be fixed for Windows compilation.
#
# Define SDL2_INCLUDE_DIRS manually, and point SDL2_LIBRARY to SDL2.lib and
# SDL2main.lib. The project may need to be edited.
find_package (SDL2 REQUIRED)
include_directories (${SDL2_INCLUDE_DIRS})

# The below needs to be fixed for Windows compilation.
#
# Define FREETYPE_INCLUDE_DIRS manually, and point FREETYPE_LIBRARIES to
# freetyped.lib.
find_package (Freetype REQUIRED)
include_directories (${FREETYPE_INCLUDE_DIRS})

set (SOURCES Console.cc)

add_executable (${PROJECT_NAME} ${SOURCES})
target_link_libraries (Console ${OPENGL_LIBRARIES})
target_link_libraries (Console ${SDL2_LIBRARY})
target_link_libraries (Console ${FREETYPE_LIBRARIES})
