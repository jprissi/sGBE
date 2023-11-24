cmake_minimum_required(VERSION 3.10)
project(GBE)

find_package(Python COMPONENTS Interpreter REQUIRED)

add_subdirectory(${PROJECT_SOURCE_DIR}/src bin)
target_include_directories(gbe PRIVATE include)
