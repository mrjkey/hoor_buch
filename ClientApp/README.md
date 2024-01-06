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
