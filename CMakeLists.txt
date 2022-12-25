cmake_minimum_required(VERSION 3.18)

project(controller
        VERSION 0.1.0
        DESCRIPTION "Allows for manual control of one of April's Creatures"
        HOMEPAGE_URL https://github.com/opsnlops/creature-controller
        LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

find_package(PkgConfig REQUIRED)

# This project uses threads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# OpenLighting, used for DMX
pkg_check_modules(OLA REQUIRED
        libola
        )

add_definitions(-DLOG_USE_COLOR)

# Our source files
file(GLOB sourceFiles
        src/*
        src/creature/*
        lib/log.c/src/*
        )

include_directories(
        src
        lib/log.c/src
        )

add_executable(controller
        ${sourceFiles}
        .
        )

target_include_directories(controller
        PUBLIC ${OLA_INCLUDE_DIRS}
        )

target_compile_options(controller
        PUBLIC ${OLA_CFLAGS_OTHER}
        )

target_link_libraries(controller
        PRIVATE ${OLA_LIBRARIES}
        Threads::Threads
        )
