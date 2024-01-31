#########################################################
# CMake File for the Central64 Examples and Analysis Code
#########################################################
cmake_minimum_required(VERSION 3.10)
project(Central64)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

if(MSVC)
  add_compile_options(/MP)
  add_compile_options(/std:c++${CMAKE_CXX_STANDARD})
  string(REGEX REPLACE "/Z[iI7]" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Z7 /bigobj")
else()
  set(WARNING_FLAGS "-Wall -Wextra -Wno-unused-variable -Wno-unused-parameter -Wno-sign-compare -Wno-missing-braces -Wno-ignored-attributes -Wignored-qualifiers -Woverloaded-virtual")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -pedantic-errors -std=c++${CMAKE_CXX_STANDARD} ${WARNING_FLAGS}")
  set(CMAKE_STATIC_LINKER_FLAGS  "${CMAKE_STATIC_LINKER_FLAGS}")
endif(MSVC)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX -debug)
endif()

include_directories("include")

set(GRID_DIR "include/central64/grid")
file(GLOB GRID_HDRS "${GRID_DIR}/*.hpp")
source_group("grid" FILES ${GRID_HDRS})

set(SEARCH_DIR "include/central64/search")
file(GLOB SEARCH_HDRS "${SEARCH_DIR}/*.hpp")
source_group("search" FILES ${SEARCH_HDRS})

set(SMOOTHING_DIR "include/central64/smoothing")
file(GLOB SMOOTHING_HDRS "${SMOOTHING_DIR}/*.hpp")
source_group("smoothing" FILES ${SMOOTHING_HDRS})

set(IO_DIR "include/central64/io")
file(GLOB IO_HDRS "${IO_DIR}/*.hpp")
source_group("io" FILES ${IO_HDRS})

set(CENTRAL64_DIR "include/central64")
file(GLOB CENTRAL64_HDRS "${CENTRAL64_DIR}/*.hpp")
source_group("central64" FILES ${CENTRAL64_HDRS})

set(EXAMPLES_DIR examples) 
include_directories(${EXAMPLES_DIR})
file(GLOB EXAMPLES_HDRS "${EXAMPLES_DIR}/*.hpp")
aux_source_directory(${EXAMPLES_DIR} EXAMPLES_SRCS)
add_executable(Central64Examples ${EXAMPLES_SRCS} ${EXAMPLES_HDRS} ${GRID_HDRS} ${SEARCH_HDRS} ${SMOOTHING_HDRS} ${CENTRAL64_HDRS})

set(ANALYSIS_DIR analysis) 
include_directories(${ANALYSIS_DIR})
file(GLOB ANALYSIS_HDRS "${ANALYSIS_DIR}/*.hpp")
aux_source_directory(${ANALYSIS_DIR} ANALYSIS_SRCS)
add_executable(Central64Analysis ${ANALYSIS_SRCS} ${ANALYSIS_HDRS} ${GRID_HDRS} ${SEARCH_HDRS} ${SMOOTHING_HDRS} ${IO_HDRS} ${CENTRAL64_HDRS})
