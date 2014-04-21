project(cellcpp)
cmake_minimum_required(VERSION 2.8)

add_definitions("-std=c++11 -Wall")

include_directories("Libraries/GoogleMock/include/")

add_library(gmock STATIC Libraries/GoogleMock/src/gmock-gtest-all.cc)

add_executable(cellcpptest Test/main.cpp)

target_link_libraries(cellcpptest gmock pthread)