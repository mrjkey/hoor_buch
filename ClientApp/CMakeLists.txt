# hoor_buch/ClientApp/CMakeLists.txt


# hoor_buch/ClientApp/CMakeLists.txt

# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.10)

# Setup the toolchain file for windows or linux, depending on what system I am running this on

if(WIN32)
    set(CMAKE_TOOLCHAIN_FILE "C:/Users/B-Itch/code/hoor_buch/ClientApp/vcpkg/scripts/buildsystems/vcpkg.cmake")
elseif(UNIX)
    set(CMAKE_TOOLCHAIN_FILE "/home/kej7be/hub/hoor_buch/ClientApp/vcpkg/scripts/buildsystems/vcpkg.cmake")
endif()

# Ignore all warnings, because fuck it
if(MSVC)
    add_compile_options(/w)
endif()


# Project's name
project(ClientApp)

# Set the C++ standard you wish to use (you could use 11, 14, 17, etc.)
set(CMAKE_CXX_STANDARD 17)

# Create a library with your shared code
file(GLOB SOURCES "src/*.cpp")
add_executable(ClientApp ${SOURCES})

# Add ImGui source files
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
add_library(imgui
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp

    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Set this to the path where GLFW's include directory is located
if(WIN32)
set(GLFW_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/installed/x64-windows/include")
elseif(UNIX)
set(GLFW_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/installed/x64-linux/include")
endif()

# Include directories for ImGui
target_include_directories(imgui PUBLIC 
    ${IMGUI_DIR} 
    ${IMGUI_DIR}/backends
    ${GLFW_INCLUDE_DIRS}  # make sure this points to the correct GLFW directory
)

# set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/glfw-3.3.9)
# set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/glfw-3.3.9/build/src/Debug")

# Add the include directory for GLFW
# include_directories("${CMAKE_CURRENT_SOURCE_DIR}/glfw-3.3.9/include")

# Link to the GLFW library directly
# target_link_libraries(ClientApp "${CMAKE_CURRENT_SOURCE_DIR}/glfw-3.3.9/build/src/Debug/glfw3.lib")


# Find and link GLFW
find_package(glfw3 REQUIRED)
target_link_libraries(ClientApp glfw)

# Find and link OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(ClientApp OpenGL::GL)

# Find SFML
find_package(SFML 2.5 COMPONENTS audio REQUIRED)

# Link SFML
target_link_libraries(ClientApp sfml-audio)


# Link ImGui with your application
target_link_libraries(ClientApp imgui)

# Add ImGuiFileDialog
add_subdirectory(ImGuiFileDialog)

# Link ImGuiFileDialog with your application
target_link_libraries(ClientApp ImGuiFileDialog)

# Add ImGui include directory to ImGuiFileDialog
target_include_directories(ImGuiFileDialog PRIVATE ${IMGUI_DIR})

# Find Yaml-cpp
find_package(yaml-cpp CONFIG REQUIRED)

# Link Yaml-cpp
target_link_libraries(ClientApp yaml-cpp::yaml-cpp)



# Find and link taglib
find_package(PkgConfig)
pkg_check_modules(TAGLIB REQUIRED IMPORTED_TARGET taglib)
target_link_libraries(ClientApp PkgConfig::TAGLIB)


# # Add the path to libvlcpp includes
# include_directories("libvlcpp")

# # If you have added libvlcpp sources to your project, you might also need to add them to your target
# file(GLOB LIBVLCPP_SOURCES "libvlcpp/*.cpp")
# target_link_libraries(ClientApp  ${LIBVLCPP_SOURCES})

# # Link against libVLC
# # Set the path to the VLC installation
# set(VLC_INSTALL_PATH "C:\\Program Files\\VideoLAN\\VLC")

# # Link against libvlc and libvlccore
# find_library(LIBVLC_LIBRARY NAMES libvlc PATHS ${VLC_INSTALL_PATH})
# find_library(LIBVLCCORE_LIBRARY NAMES libvlccore PATHS ${VLC_INSTALL_PATH})
# target_link_libraries(ClientApp ${LIBVLC_LIBRARY} ${LIBVLCCORE_LIBRARY})

# find_library(VLC_LIBRARY REQUIRED)
# target_link_libraries(ClientApp ${VLC_LIBRARY})


# Conditionally set the Boost root and library directories based on the OS
if(WIN32)
    # Set the Boost root (path to the Boost installation).
    # set(BOOST_ROOT C:/Boost/boost_1_84_0)
    # You can also set the library directory if needed
    # set(BOOST_LIBRARYDIR C:/Boost/boost_1_84_0/stage/lib)

    # set the win32_winnt to 0x0601
    add_definitions(-D_WIN32_WINNT=0x0601)

    # Find Boost on your system
    if(POLICY CMP0074)
        cmake_policy(SET CMP0074 NEW)
    endif()

    find_package(Boost 1.84.0 REQUIRED COMPONENTS)
elseif(UNIX AND NOT APPLE)
    # For Ubuntu, you might not need to explicitly set the Boost root if installed via package manager.
    # If you installed Boost manually to a custom location, set it here.
    # set(BOOST_ROOT /path/to/boost)
    # Find Boost on your system. Specify the minimum required version.
find_package(Boost 1.74.0 REQUIRED COMPONENTS system thread)
endif()

# find_package(PkgConfig REQUIRED)
# pkg_check_modules(GTK3 REQUIRED)

# include_directories(${GTK3_INCLUDE_DIRS})
# link_directories(${GTK3_LIBRARY_DIRS})

# add_definitions(${GTK3_CFLAGS_OTHER})

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})

    # Conditionally link libraries based on the OS
    if(WIN32)
        # Link against the Winsock library for Boost.Asio on Windows
        target_link_libraries(ClientApp ${Boost_LIBRARIES} Ws2_32 )
    elseif(UNIX AND NOT APPLE)
        # On Ubuntu, you generally just need the Boost libraries
        target_link_libraries(ClientApp ${Boost_LIBRARIES} )
    endif()
endif()




# If you have include files, you can add them here
target_include_directories(ClientApp PUBLIC include)

# Enable testing with CTest
enable_testing()

# Include Google Test
# add_subdirectory(googletest)
# Find the Google Test package
find_package(GTest CONFIG REQUIRED)

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