cmake_minimum_required(VERSION 3.14)
project(sotest VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/CPM.cmake)

set(SOURCES
    src/main.c
    src/str.c
    src/command.c
    src/parse.c
)

add_executable(sotest ${SOURCES})
