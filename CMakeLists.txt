cmake_minimum_required(VERSION 3.20)
project(Astracare VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ASTRACARE_BUILD_UI    "Build the optional Qt desktop application" OFF)
option(ASTRACARE_BUILD_TESTS "Build unit tests"                 ON)

add_subdirectory(core)
add_subdirectory(storage)

if(ASTRACARE_BUILD_UI)
    add_subdirectory(ui)
endif()

if(ASTRACARE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
