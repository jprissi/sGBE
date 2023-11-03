cmake_minimum_required(VERSION 3.10)
project(GBE)

## Install the executable to <install_directory>/bin
# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

## Install libraries (if any) to <install_directory>/bin
# set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

find_package(Python COMPONENTS Interpreter REQUIRED)
add_custom_command(
    # TARGET gbe
    OUTPUT opcodes.hpp
    PRE_BUILD # Call this command pre-build
    COMMAND PYTHONPATH=${PROJECT_SOURCE_DIR} ${Python_EXECUTABLE} ${PROJECT_SOURCE_DIR}/generate_opcodes.py
    COMMENT "Generate Opcodes header file"
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)    

add_subdirectory(${PROJECT_SOURCE_DIR}/src bin)
target_include_directories(gbe PRIVATE include)

