cmake_minimum_required(VERSION 3.0)

project(DFG)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost 1.59 COMPONENTS system filesystem REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

add_subdirectory(test)
add_test(NAME DFGTester COMMAND test/Test)

enable_testing()
