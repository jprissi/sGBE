cmake_minimum_required( VERSION 3.10 )

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file( GLOB SOURCES *.cpp )

# set(CMAKE_CXX_FLAGS "-Wall -Wextra" )
set(CMAKE_CXX_FLAGS "-Wall" )

set( MAIN_SOURCES main.cpp )
set ( MAIN_EXE_NAME gbe )


add_executable( ${MAIN_EXE_NAME} ${MAIN_SOURCES} ${SOURCES} )
target_link_libraries( ${MAIN_EXE_NAME} )

install( TARGETS ${MAIN_EXE_NAME} RUNTIME DESTINATION bin )