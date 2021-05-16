cmake_minimum_required(VERSION 3.19)
project(Solution)

set(CMAKE_CXX_STANDARD 14)

set(INC_DIR /home/haowong/CLionProjects/Solution)
include_directories(${INC_DIR})

aux_source_directory(. DIR_SRCS)
#FILE(GLOB DIR_SRCS "*.cpp")

add_executable(Solution ${DIR_SRCS})