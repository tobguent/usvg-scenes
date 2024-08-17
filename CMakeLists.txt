cmake_minimum_required(VERSION 3.10)
project(usvg-scenes)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)

# Add the tinyxml2 library
include(FetchContent)
FetchContent_Declare(tinyxml2
	GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
	)
set(tinyxml2_BUILD_TESTING OFF CACHE BOOL "Build tests for tinyxml2")
FetchContent_MakeAvailable(tinyxml2)
set_target_properties(tinyxml2
	PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
	)

# Add the executable and include the header file 
add_executable(usvg-scenes main.cpp reader.hpp)

# Link against tinyxml2
target_link_libraries(usvg-scenes PRIVATE tinyxml2)
