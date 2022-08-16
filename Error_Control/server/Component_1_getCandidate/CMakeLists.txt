cmake_minimum_required(VERSION 3.24)
project(Component_1_getCandidate)

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

set(INC_DIR1 Componet_1_getCandidate/inc) #https://blog.csdn.net/u013870094/article/details/78153408
set(INC_DIR2 tools/inc)
set(INC_DIR3 arduino-lmic/inc)

include_directories(${INC_DIR1}
        ${INC_DIR2}
        ${INC_DIR3})

aux_source_directory(Componet_1_getCandidate/src SRCS_DIR1) #https://www.cnblogs.com/areful/p/12170321.html
aux_source_directory(tools/src SRCS_DIR2)
aux_source_directory(arduino-lmic/src SRCS_DIR3)

add_executable(Component_1_getCandidate
        main.cpp
        ${SRCS_DIR1}
        ${SRCS_DIR2}
        ${SRCS_DIR3})

# 寻找对应的库
llvm_map_components_to_libnames(llvm_libs ${LLVM_LINK_COMPONENTS})

# 将库与目标代码一起链接
target_link_libraries(Component_1_getCandidate ${llvm_libs})