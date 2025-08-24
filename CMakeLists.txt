cmake_minimum_required(VERSION 3.30)

set(CMAKE_CXX_SCAN_FOR_MODULES ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(carbon LANGUAGES CXX)
option(CARBON_TEST "Build tests" OFF)

add_subdirectory(src)

if(CARBON_TEST)
    add_subdirectory(tests)
endif()