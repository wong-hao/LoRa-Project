cmake_minimum_required(VERSION 3.19)
project(Component_1_socketexample)

set(CMAKE_CXX_STANDARD 14)

set(INC_DIR inc) #https://blog.csdn.net/u013870094/article/details/78153408
include_directories(${INC_DIR})

add_executable(Component_1_socketexample
        main.cpp)