cmake_minimum_required(VERSION 3.16)
project(nanofmt VERSION 0.1 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(NANOFMT_NS "" CACHE STRING "Namespace for nanofmt API and implementation")

add_library(nanofmt STATIC)
add_library(nanofmt::nanofmt ALIAS nanofmt)
target_include_directories(nanofmt PUBLIC include)
target_compile_features(nanofmt PUBLIC cxx_std_17)

if (NANOFMT_NS)
    target_compile_definitions(nanofmt PUBLIC -DNANOFMT_NS=${NANOFMT_NS})
endif()

add_subdirectory(source)
add_subdirectory(include/nanofmt)
add_subdirectory(tests)
