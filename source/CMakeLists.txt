add_executable(cell main.cpp)
target_link_libraries(cell ${Boost_LIBRARIES} cellconfig)
install(TARGETS cell RUNTIME DESTINATION bin)

add_subdirectory("config")