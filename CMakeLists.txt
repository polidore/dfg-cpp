cmake_minimum_required(VERSION 3.0)

project(DFG)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost 1.55 COMPONENTS system filesystem REQUIRED)

add_library(dfg ./src/dfg.cpp)
target_link_libraries(dfg ${Boost_LIBRARIES})

add_subdirectory(test)
add_test(NAME DFGTester COMMAND test/Test)

enable_testing()
