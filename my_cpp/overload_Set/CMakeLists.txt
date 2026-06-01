cmake_minimum_required(VERSION 3.20)


project(overload_set)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(overload_set test.cpp)
add_executable(npower n_power.cpp)
add_executable(call Callable.cpp)