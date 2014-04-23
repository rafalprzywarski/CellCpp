project(cellcpp)
cmake_minimum_required(VERSION 2.8)

find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_definitions("-std=c++11 -Wall")

include_directories("libraries/GoogleMock/include/")

add_library(gmock STATIC libraries/GoogleMock/src/gmock-gtest-all.cc)

add_executable(celltest test/main.cpp)
add_executable(cell source/main.cpp)

target_link_libraries(celltest gmock pthread)
target_link_libraries(cell ${Boost_LIBRARIES})

install(TARGETS cell RUNTIME DESTINATION bin)
