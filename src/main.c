#include <main.h>


fd_set READSET;  // 监听集合
int g_maxfd = 0;  // 最大文件描述符
Node *head = NULL; // 链表头

int main(int argc, char *argv[])
{
    // 清理共享内存
    system("../init.sh");

    // 1. select初始化
    init_select();

    // 2. 初始化设备文件
    if (init_dirver() == -1) {
        printf("init_dirver error\n");
        return -1;
    }

    // 3. 共享内存
    if (init_shm() == -1) {
        printf("init_shm error\n");
        return -1;
    }

    // 4. 初始化双向链表 (保存歌曲名字)
    if (link_init() == -1) {
        printf("link_init error\n");
        return -1;
    }

    // 5. 初始化socket
    if (init_socket() == -1) {
        printf("init_socket error\n");
        return -1;
    }



    return 0;

}
