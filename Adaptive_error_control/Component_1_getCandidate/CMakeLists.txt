cmake_minimum_required(VERSION 3.20)
project(Component_1_getCandidate)

set(CMAKE_CXX_STANDARD 14)

set(INC_DIR Componet_1_getCandidate/inc) #https://blog.csdn.net/u013870094/article/details/78153408
include_directories(${INC_DIR})

aux_source_directory(Componet_1_getCandidate/src SRCS_DIR1) #https://www.cnblogs.com/areful/p/12170321.html

add_executable(Component_1_getCandidate
        main.cpp
        ${SRCS_DIR1})