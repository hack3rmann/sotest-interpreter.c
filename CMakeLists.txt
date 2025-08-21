cmake_minimum_required(VERSION 3.14)
project(sotest VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/CPM.cmake)

CPMAddPackage("gh:LeoVen/C-Macro-Collections@0.26.0")

set(SOURCES
    src/main.c
    src/str.c
    src/parse.c
    src/interpreter.c
)

add_executable(sotest ${SOURCES})

target_link_libraries(sotest PRIVATE dl)

target_include_directories(sotest PRIVATE ${cmc_SOURCE_DIR}/src)

CPMAddPackage(
    NAME cmc
    GIT_REPOSITORY https://github.com/LeoVen/C-Macro-Collections.git
    VERSION 0.26.0
    DOWNLOAD_ONLY YES
)

if (cmc_ADDED)
    target_include_directories(sotest PRIVATE ${cmc_SOURCE_DIR}/src)
endif()

target_compile_options(
    sotest PRIVATE
    -Wall
    -Wextra
    -Werror
    # Allow `Type constexpr NAME = ...` syntax (Type first)
    -Wno-old-style-declaration
)

add_library(
    test1 SHARED
    src/test-libraries/test-lib1.c
)

add_library(
    test2 SHARED
    src/test-libraries/test-lib2.c
)
