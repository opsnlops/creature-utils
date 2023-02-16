cmake_minimum_required(VERSION 3.16)

project(creature-utils
        VERSION 0.2.0
        DESCRIPTION "Utilities for controlling April's Creatures"
        HOMEPAGE_URL https://github.com/opsnlops/creature-utils
        LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

find_package(PkgConfig REQUIRED)

# This project uses threads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


add_definitions(-DLOG_USE_COLOR)

# Our source files
file(GLOB COMMON_SOURCES
        lib/log.c/src/*
        lib/libe131/src/*
        )

set(JOYSTICK_SOURCES
        src/dmx/dmx.cpp
        src/common/config.cpp
        src/joystick/display.cpp
        src/joystick/joystick.cpp
        src/joystick/main.cpp
        src/joystick/servo.cpp
        src/joystick/uart.cpp
        )

set(STRESS_TESTS_SOURCES
        src/common/config.cpp
        src/stress-test/main.cpp
        )


include_directories(
        src/
        lib/log.c/src
        lib/libe131/src
        )

add_executable(joystick
        ${COMMON_SOURCES}
        ${JOYSTICK_SOURCES}
        )

add_executable(stress-test
        ${COMMON_SOURCES}
        ${STRESS_TESTS_SOURCES}
        )

target_link_libraries(joystick
        Threads::Threads
        )