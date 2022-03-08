cmake_minimum_required(VERSION 3.23)
project(Component_1_fakesend)

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

set(INC_DIR inc) #https://blog.csdn.net/u013870094/article/details/78153408
include_directories(${INC_DIR})


#aux_source_directory(. SRCS_DIR) #https://www.cnblogs.com/tangxiaosheng/p/7727593.html
#FILE(GLOB SRCS_DIR "*.cpp") #https://www.cnblogs.com/dilex/p/11102152.html
aux_source_directory(src SRCS_DIR) #https://www.cnblogs.com/areful/p/12170321.html

add_executable(Component_1_fakesend
        main.cpp
        ${SRCS_DIR})

# 寻找对应的库
llvm_map_components_to_libnames(llvm_libs ${LLVM_LINK_COMPONENTS})

# 将库与目标代码一起链接
target_link_libraries(Component_1_fakesend ${llvm_libs})