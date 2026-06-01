cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_COMPILER /usr/bin/clang-18)
set(CMAKE_CXX_COMPILER /usr/bin/clang++-18)
project(cpp)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(overload_Set)
add_subdirectory(ringbuffer)
add_subdirectory(overload_resolution)