include_directories(${GoogleMock_INCLUDE_DIRS})
include_directories("../source")

add_executable(celltest main.cpp config/config_test.cpp config/parse_properties_test.cpp)
target_link_libraries(celltest cellconfig ${Boost_LIBRARIES} gmock pthread)
