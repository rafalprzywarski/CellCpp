include_directories(${GoogleMock_INCLUDE_DIRS})

add_executable(celltest main.cpp)
target_link_libraries(celltest gmock pthread)
