project(cellcpp)
cmake_minimum_required(VERSION 2.8)

add_definitions("-std=c++11")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

set(GoogleMock_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/libraries/GoogleMock/include")

add_subdirectory("source")
add_subdirectory("test")
add_subdirectory("libraries")
