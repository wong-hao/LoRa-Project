cmake_minimum_required(VERSION 3.19)
project(Solution_epoll)

set(CMAKE_CXX_STANDARD 14)

set(INC_DIR inc) #https://blog.csdn.net/u013870094/article/details/78153408
include_directories(${INC_DIR})


#aux_source_directory(. SRCS_DIR) #https://www.cnblogs.com/tangxiaosheng/p/7727593.html
#FILE(GLOB SRCS_DIR "*.cpp") #https://www.cnblogs.com/dilex/p/11102152.html
aux_source_directory(src SRCS_DIR) #https://www.cnblogs.com/areful/p/12170321.html

add_executable(Solution_epoll
        main.cpp
        ${SRCS_DIR})