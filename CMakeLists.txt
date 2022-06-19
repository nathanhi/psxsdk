cmake_minimum_required(VERSION 3.4.3)
project(psxsdk C ASM)

include(package.cmake)

if(NOT DEFINED PSX)
    # GCC Toolchain
    add_subdirectory(toolchain toolchain)

    # Host tools
    add_subdirectory(tools tools)

    include(ExternalProject)
    ExternalProject_Get_Property(gcc INSTALL_DIR)
    ExternalProject_Add(psxsdk
        SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}
        DEPENDS gcc
        BUILD_ALWAYS TRUE
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
                   -DCMAKE_MODULE_PATH:PATH=${CMAKE_CURRENT_LIST_DIR}/toolchain/cmake/Modules
                   -DCMAKE_SYSROOT:PATH=${INSTALL_DIR}
                   -DCMAKE_SYSTEM_NAME:STRING=psx
    )
    return()
endif()

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs libs)