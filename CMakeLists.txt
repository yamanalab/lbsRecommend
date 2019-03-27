cmake_minimum_required(VERSION 2.8)

set(project_name lsbrecommend)
project(${project_name})

find_package(Threads REQUIRED)
set(CMAKE_CXX_FLAGS "-O3 -std=c++11 -pthread -Wall -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O2 -g -std=c++11 -pthread -Wall")

set(HELIB_DIR /usr/local/src/HElib/src)
include_directories(${PROJECT_SOURCE_DIR}/lbsr ${PROJECT_SOURCE_DIR}/stdsc ${HELIB_DIR})
link_directories(${HELIB_DIR})

set(COMMON_LIBS stdsc lbsr_share helib ntl gmp)

add_subdirectory(stdsc)
add_subdirectory(lbsr)
add_subdirectory(demo)
