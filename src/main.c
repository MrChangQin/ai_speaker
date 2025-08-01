#include <main.h>



fd_set READSET;  // 监听集合


int main(int argc, char *argv[])
{

    // select初始化
    init_select();

    // 初始化设备文件
    if(init_dirver() == -1){
        printf("init_dirver error\n");
        // exit(1);
    }

    return 0;

}
