cmake_minimum_required(VERSION 3.18)

project(creature-utils
        VERSION 0.1.0
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
file(GLOB commonFiles
        src/*
        src/creature/*
        src/dmx/*
        src/joystick/*
        lib/log.c/src/*
        lib/libe131/src/*
        )

include_directories(joystick
        src
        lib/log.c/src
        lib/libe131/src
        )

add_executable(joystick
        ${commonFiles}
        joystick/
        dmx/
        creature/
        .
        )

target_link_libraries(joystick
        PRIVATE
        Threads::Threads
        )
