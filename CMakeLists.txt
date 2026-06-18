cmake_minimum_required(VERSION 3.20)
project(qcdot VERSION 0.1.0 LANGUAGES CXX)

# QCdot - Visual Programming Engine and Editor

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# MSVC requires /Zc:__cplusplus and /permissive- for Qt6
if(MSVC)
    add_compile_options(/Zc:__cplusplus /permissive-)
endif()

# Find Qt6
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Widgets)

# Subdirectories
add_subdirectory(engine)
add_subdirectory(nodes)
add_subdirectory(editor)
add_subdirectory(examples)

# Tests (optional)
enable_testing()
add_subdirectory(tests)