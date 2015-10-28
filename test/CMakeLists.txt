cmake_minimum_required(VERSION 3.0)

add_executable(Test bootstrap.cpp dfg.spec.cpp)
add_executable(Play bootstrap.cpp ptree.spec.cpp)
set_property(TARGET Test PROPERTY CXX_STANDARD 11)
set_property(TARGET Play PROPERTY CXX_STANDARD 11)
target_link_libraries(Test ${Boost_LIBRARIES})
target_link_libraries(Play ${Boost_LIBRARIES})
