add_definitions("-Wall -Wno-unused-local-typedef")

add_executable(cell ConfiguredCompiler.cpp NativeCommandExecutor find_cpp_files.cpp main.cpp)
target_link_libraries(cell ${Boost_LIBRARIES} cellconfig)
install(TARGETS cell RUNTIME DESTINATION bin)

add_subdirectory("config")
