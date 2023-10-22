cmake_minimum_required(VERSION 3.10)
project(GBE)

## Install the executable to <install_directory>/bin
# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

## Install libraries (if any) to <install_directory>/bin
# set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

add_subdirectory(${PROJECT_SOURCE_DIR}/src bin)
target_include_directories(gbe PRIVATE include)