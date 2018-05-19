cmake_minimum_required(VERSION 3.8)

# add this options before PROJECT keyword
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(cwap)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# we have to use clang...
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rd-party)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)

