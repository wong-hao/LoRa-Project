cmake_minimum_required(VERSION 3.20)
project(Component_3_connectton_error_correction)

set(CMAKE_CXX_STANDARD 14)

#https://www.codercto.com/a/23570.html
#https://blog.csdn.net/suningning/article/details/101207679
#all libraries
set(LLVM_LINK_COMPONENTS
        all
        )

# 使用llvm来设置路径
find_package(LLVM REQUIRED CONFIG)

# 打印信息
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# 添加头文件包含路径
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

set(INC_DIR1 Component_1_fakesend/inc) #https://blog.csdn.net/u013870094/article/details/78153408
set(INC_DIR2 Component_1_getCandidate/inc)
set(INC_DIR3 Component_1_getMask/inc)
set(INC_DIR4 Component_2_server_side/inc)
set(INC_DIR5 Component_2_error_correction/inc)
set(INC_DIR6 Component_3_connectton_error_correction/inc)
set(INC_DIR7 tools/inc)
set(INC_DIR8 arduino-lmic/inc)
include_directories(${INC_DIR1}
        ${INC_DIR2}
        ${INC_DIR3}
        ${INC_DIR4}
        ${INC_DIR5}
        ${INC_DIR6}
        ${INC_DIR7}
        ${INC_DIR8})

#aux_source_directory(. SRCS_DIR) #https://www.cnblogs.com/tangxiaosheng/p/7727593.html
#FILE(GLOB SRCS_DIR "*.cpp") #https://www.cnblogs.com/dilex/p/11102152.html
aux_source_directory(Component_1_fakesend/src SRCS_DIR1) #https://www.cnblogs.com/areful/p/12170321.html
aux_source_directory(Component_1_getCandidate/src SRCS_DIR2)
aux_source_directory(Component_1_getMask/src SRCS_DIR3)
aux_source_directory(Component_2_server_side/src SRCS_DIR4)
aux_source_directory(Component_2_error_correction/src SRCS_DIR5)
aux_source_directory(Component_3_connectton_error_correction/src SRCS_DIR6)
aux_source_directory(tools/src SRCS_DIR7)
aux_source_directory(arduino-lmic/src SRCS_DIR8)

add_executable(Component_3_connectton_error_correction
        APC.cpp
        ${SRCS_DIR1}
        ${SRCS_DIR2}
        ${SRCS_DIR3}
        ${SRCS_DIR4}
        ${SRCS_DIR5}
        ${SRCS_DIR6}
        ${SRCS_DIR7}
        ${SRCS_DIR8})

# 寻找对应的库
llvm_map_components_to_libnames(llvm_libs ${LLVM_LINK_COMPONENTS})

# 将库与目标代码一起链接
target_link_libraries(Component_3_connectton_error_correction ${llvm_libs})