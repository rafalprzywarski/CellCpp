project(cellcpp)
cmake_minimum_required(VERSION 2.8)

add_definitions("-std=c++11 -Wall")

include_directories("libraries/GoogleMock/include/")

add_library(gmock STATIC libraries/GoogleMock/src/gmock-gtest-all.cc)

add_executable(celltest test/main.cpp)

add_executable(cell source/main.cpp)

target_link_libraries(celltest gmock pthread)

install(TARGETS cell RUNTIME DESTINATION bin)
