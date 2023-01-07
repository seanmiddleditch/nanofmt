cmake_minimum_required(VERSION 3.25)
project(nanofmt VERSION 0.2 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(NANOFMT_IS_TOPLEVEL FALSE)
if (${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
    set(NANOFMT_IS_TOPLEVEL TRUE)
endif()

set(NANOFMT_NS "" CACHE STRING "Namespace for nanofmt API and implementation")

option(NANOFMT_TESTS "Build nanofmt tests" ${NANOFMT_IS_TOPLEVEL})
option(NANOFMT_DOCS "Build sphinx and doxygen documentation" OFF)
option(NANOFMT_INSTALL "Add install targets for nanofmt" ON)
option(NANOFMT_FLOAT "Enable support for float and double" ON)

add_library(nanofmt STATIC)
add_library(nanofmt::nanofmt ALIAS nanofmt)
target_include_directories(nanofmt
    PRIVATE
        include
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_compile_features(nanofmt PUBLIC cxx_std_17)

if (NANOFMT_FLOAT)
    target_compile_definitions(nanofmt PUBLIC -DNANOFMT_FLOAT=1)
endif()

if (NANOFMT_NS)
    target_compile_definitions(nanofmt PUBLIC -DNANOFMT_NS=${NANOFMT_NS})
endif()

add_subdirectory(source)
add_subdirectory(include/nanofmt)

if (NANOFMT_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if (NANOFMT_DOCS)
    add_subdirectory(docs)
endif()

if (NANOFMT_INSTALL)
    install(TARGETS nanofmt DESTINATION lib EXPORT nanofmtTargets)
    install(DIRECTORY include/nanofmt
        TYPE INCLUDE
        FILES_MATCHING
        PATTERN "*.h"
    )
    install(EXPORT nanofmtTargets
        NAMESPACE nanofmt
        FILE nanofmtTargets.cmake
        DESTINATION lib/cmake/nanofmt
    )

    include(CMakePackageConfigHelpers)
    configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/nanofmtConfig.cmake"
        INSTALL_DESTINATION "lib/cmake/example"
        NO_SET_AND_CHECK_MACRO
        NO_CHECK_REQUIRED_COMPONENTS_MACRO
    )
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/nanofmtConfigVersion.cmake"
        VERSION "${nanofmt_VERSION_MAJOR}.${nanofmt_VERSION_MINOR}"
        COMPATIBILITY AnyNewerVersion
    )

    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/nanofmtConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/nanofmtConfigVersion.cmake
        DESTINATION lib/cmake/nanofmt
    )
endif()
