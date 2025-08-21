#!/bin/sh

# build project
echo -e "\033[0;35mbuilding project!!!\033[0m"
mkdir -p build  # 创建build目录（如果不存在）
cd build
cmake ..
make
echo -e "\033[0;35mbuild finished!!!\033[0m"

# run main.exe
echo -e "\033[0;35mrunning executable file!!!\033[0m"
cd ..
cd bin || exit
./main