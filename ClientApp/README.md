# ClientApp

This is a brief description of your project.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

What things you need to install the software and how to install them.

<!-- Installing -->

### Installing

A step by step series of examples that tell you how to get a development environment running.

Running the tests
Explain how to run the automated tests for this system.

Built With
PortAudio - The audio library used
CMake - Build system
Contributing
Please read CONTRIBUTING.md for details on our code of conduct, and the process for submitting pull requests to us.

License
This project is licensed under the MIT License - see the LICENSE.md file for details

Acknowledgments
Hat tip to anyone whose code was used
Inspiration
etc

# Setup

### get google test

```
git clone https://github.com/google/googletest.git
```

### get boost

```
sudo apt-get install libboost-all-dev
```

# build

```
cd build
cmake ..
make
```

```
cmake --build .
```

```
cmake --build . --target test
```

```
cmake --build . --config Release
```

```
cmake -G "MinGW Makefiles" ..
mingw32-make

```

.\ClientApp.exe ::1 6969 server

# Connecting...

## windows machine???

2a02:8109:b69a:dd00:7d8:1952:918d:4c0b

## linux machine ???

2a02:8109:b69a:dd00:ce08:82ef:c14c:2bf9

2a02:8109:b69a:dd00:a064:2538:66f:abfd

# ImGui

https://github.com/ocornut/imgui.git

git clone --recursive https://github.com/ocornut/imgui.git -b docking

git clone https://github.com/glfw/glfw.git

git clone https://github.com/Microsoft/vcpkg.git

.\vcpkg\bootstrap-vcpkg.bat

.\vcpkg\vcpkg.exe install glfw3

./bootstrap-vcpkg.sh

.\vcpkg.exe integrate install

.\vcpkg.exe install gtest

./vcpkg install libsndfile

./vcpkg install portaudio

..\vcpkg\vcpkg.exe install taglib
