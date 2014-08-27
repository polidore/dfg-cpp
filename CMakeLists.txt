cmake_minimum_required(VERSION 3.0)

project(DFG)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_library(dfg ./src/dfg.cpp)
add_subdirectory(test)
add_test(NAME DFGTester COMMAND test/Test)

enable_testing()
