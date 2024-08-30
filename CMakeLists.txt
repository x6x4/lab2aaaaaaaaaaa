
cmake_minimum_required(VERSION 3.16)

set (CMAKE_CXX_COMPILER "/usr/bin/g++")

project(TA_lab2)

set (CMAKE_CXX_STANDARD 20)
set (AUX_FLAGS "-O0 -coverage ")


set (CMAKE_CXX_FLAGS "-g -std=c++20 -Wall -Wextra -Wpedantic")

add_subdirectory(regexTokenizer)
add_subdirectory(common)
add_subdirectory(ast)
add_subdirectory(dfa_sets)
add_subdirectory(regex)


add_executable(a.out main.cpp)
target_link_libraries(a.out regex)

add_executable(b.out diff.cpp)
target_link_libraries(b.out regex)