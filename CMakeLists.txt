cmake_minimum_required(VERSION 3.23)
project(joystick_dmx)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(joystick_dmx
        src/main.cpp src/joystick.cpp)
