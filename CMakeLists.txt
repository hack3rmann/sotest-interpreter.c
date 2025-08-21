cmake_minimum_required(VERSION 3.14)
project(sotest VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/CPM.cmake)

CPMAddPackage("gh:LeoVen/C-Macro-Collections@0.26.0")

set(SOURCES
    src/str.c
    src/parse.c
    src/interpreter.c
    src/args.c
)

add_executable(sotest src/main.c ${SOURCES})

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

target_compile_definitions(
    sotest PRIVATE
    PROJECT_VERSION="${PROJECT_VERSION}"
    PROJECT_NAME="${PROJECT_NAME}"
    PROJECT_DESCRIPTION="shared library reader language interpreter"
)

target_compile_options(
    sotest PRIVATE
    -Wall
    -Wextra
    # Allow `Type constexpr NAME = ...` syntax (Type goes first)
    -Wno-old-style-declaration
)

add_library(
    test1 SHARED
    tests/libraries/test-lib1.c
)

add_library(
    test2 SHARED
    tests/libraries/test-lib2.c
)

file(GLOB TEST_SOURCES tests/*.c)

add_executable(
    test
    tests/libtest/main.c
    ${SOURCES}
    ${TEST_SOURCES}
    tests/libtest/tests.c
)

target_compile_options(
    test PRIVATE
    -Wall
    -Wextra
    # Allow `Type constexpr NAME = ...` syntax (Type goes first)
    -Wno-old-style-declaration
)

target_compile_definitions(
    test PRIVATE
    PROJECT_VERSION="${PROJECT_VERSION}"
    PROJECT_NAME="${PROJECT_NAME}"
    PROJECT_DESCRIPTION="testing framework for sotest"
)

target_include_directories(test PRIVATE src ${cmc_SOURCE_DIR}/src)
