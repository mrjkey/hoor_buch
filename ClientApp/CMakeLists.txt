# hoor_buch/ClientApp/CMakeLists.txt

# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.10)

# Project's name
project(ClientApp)

# Set the C++ standard you wish to use (you could use 11, 14, 17, etc.)
set(CMAKE_CXX_STANDARD 17)


# Create a library with your shared code
file(GLOB SOURCES "src/*.cpp")
add_executable(ClientApp ${SOURCES})
# add_library(MathLib src/math_utils.cpp)  # Create a static library
# target_include_directories(MathLib PUBLIC include)  # Ensure the library's header files can be included


# Add executable target with source files listed
# add_executable(ClientApp src/main.cpp src/math_utils.cpp)

# If you have include files, you can add them here
target_include_directories(ClientApp PUBLIC include)

# Enable testing with CTest
enable_testing()
# Include Google Test
add_subdirectory(googletest)

# Add test subdirectory
add_subdirectory(tests)