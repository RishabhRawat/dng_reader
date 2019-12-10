cmake_minimum_required(VERSION 3.10)
project(dng_reader)

find_package(OpenMP REQUIRED)

set(CMAKE_CXX_STANDARD 17)

include_directories(LibRaw-0.19.5)
add_executable(dng_reader main.cpp)
#target_link_libraries(dng_reader ${CMAKE_SOURCE_DIR}/libraw.a)
target_link_libraries(dng_reader PRIVATE ${CMAKE_SOURCE_DIR}/libraw.a OpenMP::OpenMP_CXX)