cmake_minimum_required( VERSION 3.10 )

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file( GLOB SOURCES *.cpp )

# set(CMAKE_CXX_FLAGS "-Wall -Wextra" )
set(CMAKE_CXX_FLAGS "-Wall" )
set(CMAKE_CXX_FLAGS "-g" )

set( MAIN_SOURCES main.cpp )
set ( MAIN_EXE_NAME gbe )


add_executable( ${MAIN_EXE_NAME} ${MAIN_SOURCES} ${SOURCES} )
target_link_libraries( ${MAIN_EXE_NAME} )

find_package(Python COMPONENTS Interpreter REQUIRED)
add_custom_command(
    TARGET gbe
    PRE_BUILD # Call this command pre-build
    COMMAND PYTHONPATH=${PROJECT_SOURCE_DIR} ${Python_EXECUTABLE} ${PROJECT_SOURCE_DIR}/generate_opcodes.py
    COMMENT "Generate Opcodes header file"
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)    

install( TARGETS ${MAIN_EXE_NAME} RUNTIME DESTINATION bin )