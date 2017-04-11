cmake_minimum_required(VERSION 2.8.9)
project (Console)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "./CMake")

find_package (SDL2 REQUIRED)
include_directories (${SDL2_INCLUDE_DIRS})

find_package (Freetype REQUIRED)
include_directories (${FREETYPE_INCLUDE_DIRS})

set (SOURCES Console.cc)

add_executable (${PROJECT_NAME} ${SOURCES})
target_link_libraries (Console ${SDL2_LIBRARY})
target_link_libraries (Console ${FREETYPE_LIBRARIES})
