cmake_minimum_required(VERSION 3.19)
project(Component_2_server_side)

set(CMAKE_CXX_STANDARD 14)

set(INC_DIR1 Component_1_fakesend/inc) #https://blog.csdn.net/u013870094/article/details/78153408
set(INC_DIR2 Component_1_getCandidate/inc)
set(INC_DIR3 Component_1_getMast/inc)
set(INC_DIR4 Component_1_socketexample/inc)
set(INC_DIR5 inc/header_2.h)
set(INC_DIR6 Component_3_connection_error_correction/inc)
set(INC_DIR7 tools/inc)
include_directories(${INC_DIR1}
        ${INC_DIR2}
        ${INC_DIR3}
        ${INC_DIR4}
        ${INC_DIR5}
        ${INC_DIR6}
        ${INC_DIR7})

#aux_source_directory(. SRCS_DIR) #https://www.cnblogs.com/tangxiaosheng/p/7727593.html
#FILE(GLOB SRCS_DIR "*.cpp") #https://www.cnblogs.com/dilex/p/11102152.html
aux_source_directory(Component_1_fakesend/src SRCS_DIR1) #https://www.cnblogs.com/areful/p/12170321.html
aux_source_directory(Component_1_getCandidate/src SRCS_DIR2)
aux_source_directory(Component_1_getMast/src SRCS_DIR3)
aux_source_directory(Component_1_socketexample/src SRCS_DIR4)
aux_source_directory(Component_3_connection_error_correction/src SRCS_DIR5)
aux_source_directory(tools/src SRCS_DIR6)

add_executable(Component_2_server_side
        main.cpp
        ${SRCS_DIR1}
        ${SRCS_DIR2}
        ${SRCS_DIR3}
        ${SRCS_DIR4}
        ${SRCS_DIR5}
        ${SRCS_DIR6} inc/header_2.h src/header_2.cpp)