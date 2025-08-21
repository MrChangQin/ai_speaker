cmake_minimum_required(VERSION 3.10)

project(main VERSION 1.0.0 LANGUAGES C CXX)

add_definitions(-DARM)  # 添加这一行来定义 ARM 宏

set(CMAKE_CXX_STANDARD 17)

# 设置交叉编译工具链
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
# set(CMAKE_C_COMPILER arm-linux-gcc)
set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-linux-g++)

# 设置静态链接选项
set(CMAKE_EXE_LINKER_FLAGS "-static")

include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB SRC_LIST "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

# 设置 json-c 库路径
set(JSON_INSTALL_DIR /home/json_arm_install)
include_directories(${JSON_INSTALL_DIR}/include)
link_directories(${JSON_INSTALL_DIR}/lib)

add_executable(main ${SRC_LIST})

target_link_libraries(main 
    json
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin) # 设置可执行文件输出目录
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib) # 设置库文件输出目录
