cmake_minimum_required(VERSION 3.16)

project(compile_cmake)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CFLAGS} -DDEBUG -O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CFLAGS} -O3 -Wall")

include_directories(inc)
add_library(v3 SHARED src/v3.cpp)
add_library(particle SHARED src/particle.cpp)
add_executable(main src/main.cpp)
target_link_libraries(main PUBLIC v3 particle)
