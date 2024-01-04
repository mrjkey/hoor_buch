# hoor_buch/ClientApp/CMakeLists.txt


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

# Conditionally set the Boost root and library directories based on the OS
if(WIN32)
    # Set the Boost root (path to the Boost installation).
    set(BOOST_ROOT C:/Boost/boost_1_84_0)
    # You can also set the library directory if needed
    set(BOOST_LIBRARYDIR C:/Boost/boost_1_84_0/stage/lib)

    # Find Boost on your system
    find_package(Boost 1.84.0 REQUIRED COMPONENTS)
elseif(UNIX AND NOT APPLE)
    # For Ubuntu, you might not need to explicitly set the Boost root if installed via package manager.
    # If you installed Boost manually to a custom location, set it here.
    # set(BOOST_ROOT /path/to/boost)
    # Find Boost on your system. Specify the minimum required version.
find_package(Boost 1.74.0 REQUIRED COMPONENTS system thread)
endif()



if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})

    # Conditionally link libraries based on the OS
    if(WIN32)
        # Link against the Winsock library for Boost.Asio on Windows
        target_link_libraries(ClientApp ${Boost_LIBRARIES} Ws2_32)
    elseif(UNIX AND NOT APPLE)
        # On Ubuntu, you generally just need the Boost libraries
        target_link_libraries(ClientApp ${Boost_LIBRARIES})
    endif()
endif()

# If you have include files, you can add them here
target_include_directories(ClientApp PUBLIC include)

# Enable testing with CTest
enable_testing()

# Include Google Test
add_subdirectory(googletest)

# Add test subdirectory
add_subdirectory(tests)


# # Specify the minimum version for CMake
# cmake_minimum_required(VERSION 3.10)

# # Project's name
# project(ClientApp)

# # Set the C++ standard you wish to use (you could use 11, 14, 17, etc.)
# set(CMAKE_CXX_STANDARD 17)

# # Create a library with your shared code
# file(GLOB SOURCES "src/*.cpp")
# add_executable(ClientApp ${SOURCES})

# # # Find Boost on your system
# # find_package(Boost REQUIRED COMPONENTS system thread)
# # Set the Boost root (path to the Boost installation).
# set(BOOST_ROOT C:/Boost/boost_1_84_0)

# # You can also set the library directory if needed
# set(BOOST_LIBRARYDIR C:/Boost/boost_1_84_0/stage/lib)

# # Find Boost on your system
# find_package(Boost 1.74.0 REQUIRED COMPONENTS)

# if(Boost_FOUND)
#     include_directories(${Boost_INCLUDE_DIRS})
#     link_directories(${Boost_LIBRARY_DIRS})
#     # target_link_libraries(ClientApp ${Boost_LIBRARIES})
#     # Link against the Winsock library for Boost.Asio
#     target_link_libraries(ClientApp ${Boost_LIBRARIES} Ws2_32)
# endif()

# # If you have include files, you can add them here
# target_include_directories(ClientApp PUBLIC include)

# # Enable testing with CTest
# enable_testing()

# # Include Google Test
# add_subdirectory(googletest)

# # Add test subdirectory
# add_subdirectory(tests)