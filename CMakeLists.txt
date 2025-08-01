cmake_minimum_required(VERSION 3.10)

project(main VERSION 1.0.0 LANGUAGES C CXX)


add_definitions(-DDEBUG)


set(CMAKE_CXX_STANDARD 17)


include_directories(${PROJECT_SOURCE_DIR}/include) # 添加头文件路径
file(GLOB SRC_LIST "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c") # 查找当前目录下的所有源文件

# link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib) # 链接库文件路径
# link_libraries(libcmakel.a) # 链接静态库

# add_library(cmakel STATIC ${SRC_LIST}) # 添加静态库
# add_library(cmakel SHARED ${SRC_LIST}) # 添加动态库

add_executable(main ${SRC_LIST}) # 添加可执行文件
# target_link_libraries(CMAKEL libcmakel.dll) # 链接动态库

# 设置输出目录.
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin) # 设置可执行文件输出目录
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib) # 设置库文件输出目录
