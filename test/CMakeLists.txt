cmake_minimum_required(VERSION 3.0)

add_executable(Test dfg.bootstrap.cpp dfg.spec.cpp)
set_property(TARGET Test PROPERTY CXX_STANDARD 11)
target_link_libraries(Test ${Boost_LIBRARIES})
