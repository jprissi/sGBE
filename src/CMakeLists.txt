cmake_minimum_required( VERSION 3.10 )

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file( GLOB SOURCES *.cpp )

# set(CMAKE_CXX_FLAGS "-Wall -Wextra" )
# set(CMAKE_CXX_FLAGS "-Wall -pg" )
set(CMAKE_CXX_FLAGS "-Wall" )
# set(CMAKE_CXX_FLAGS "-O3" )
set(CMAKE_CXX_FLAGS "-g" )

set( MAIN_SOURCES main.cpp )
set ( MAIN_EXE_NAME gbe )


# add_executable( ${MAIN_EXE_NAME} ${MAIN_SOURCES} ${SOURCES} )


# SDL test

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(gbe main_gui.cpp ${SOURCES})
# target_link_libraries( ${MAIN_EXE_NAME} )
target_link_libraries(gbe ${SDL2_LIBRARIES})

# Install

install( TARGETS ${MAIN_EXE_NAME} RUNTIME DESTINATION bin )