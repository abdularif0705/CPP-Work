#
# CMake Tutorial URL:
#   https://cmake.org/cmake/help/latest/guide/tutorial/A%20Basic%20Starting%20Point.html
#
# To build code with CMake:
#   1) Place source code with CMakeLists.txt and all in a directory.
#   2) Create another directory elsewhere (preferably not in the same
#      directory as the source in step 1).
#   3) cd in to directory in step 2
#   4) Run: cmake path/to/step2/dir
#      i.e., replace path/to/step2/dir with the path to the directory
#      in step 2. This command only needs to be run once.
#   4) Run: make
#      i.e., this will build the code
#

# https://cmake.org/cmake/help/latest/command/cmake_minimum_required.html
#   * v3.12 chosen based on C++20 CMake support for CXX_STANDARD, i.e.,
#     https://cmake.org/cmake/help/latest/prop_tgt/CXX_STANDARD.html
cmake_minimum_required(VERSION 3.12)

# https://cmake.org/cmake/help/latest/command/project.html
project(comp3400_2023w_a5
  DESCRIPTION "UWindsor COMP3400 2023 Winter Assignment 3"
  LANGUAGES CXX
)

#
# Build provided code...
#
# https://cmake.org/cmake/help/latest/command/add_executable.html
add_executable(a5-gen-input
  a5-gen-input-provided.cpp
  a4-provided.cpp
)
set_target_properties(a5-gen-input PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)

#
# Build assignment code...
#
add_executable(a5
  a5.cpp
  a4-provided.cpp
)
set_target_properties(a5 PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)

