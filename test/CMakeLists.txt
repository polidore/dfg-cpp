cmake_minimum_required(VERSION 3.0)

add_executable(Test dfg.spec.cpp)
target_link_libraries(Test ${Boost_LIBRARIES})
