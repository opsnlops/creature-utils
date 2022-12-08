cmake_minimum_required(VERSION 3.18)
project(joystick_dmx C CXX)

set(CMAKE_CXX_STANDARD 14)

# This project uses threads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_definitions(-DLOG_USE_COLOR)

include_directories(
        src
        lib/log.c/src
)

add_executable(joystick_dmx
        src/main.cpp
        src/joystick.h
        src/joystick.cpp
        src/servo.h
        src/servo.cpp
        src/uart.cpp
        src/uart.h
        src/creature/parrot.cpp
        lib/log.c/src/log.cpp)
target_link_libraries(joystick_dmx PRIVATE
        Threads::Threads)

add_executable(parrot_test
        src/creature/parrot.cpp
        src/creature/parrot_test.cpp
        lib/log.c/src/log.cpp)
