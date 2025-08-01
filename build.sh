# build project
echo -e "\033[0;35mbuilding project!!!\033[0m"
cd build || exit
cmake ..
make
echo -e "\033[0;35mbuild finished!!!\033[0m"

# run main.exe
echo -e "\033[0;35mrunning executable file!!!\033[0m"
cd ..
cd bin || exit
./main
