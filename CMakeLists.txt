cmake_minimum_required(VERSION 3.18)
project(joystick_dmx)

set(CMAKE_CXX_STANDARD 14)

# This project uses threads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include_directories(src)

add_executable(joystick_dmx
        src/main.cpp
        src/joystick.h
        src/joystick.cpp
        src/servo.h
        src/servo.cpp)

target_link_libraries(joystick_dmx PRIVATE Threads::Threads)
