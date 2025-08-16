#include "socket.h"


extern int g_maxfd;
extern fd_set READSET;
int g_socket_fd = 0;


// typedef void (*sighandler_t)(int);

void send_info2server(int sig) {
    //TODO:

}


int init_socket(void)
{

    int count = 50;

    // 创建套接字
    g_socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (g_socket_fd == -1) {
        perror("socket");
        return -1;
    }

    // 绑定套接字
    struct sockaddr_in server_info;
    memset(&server_info, 0, sizeof(server_info));
    server_info.sin_family = PF_INET;
    server_info.sin_port = htons(PORT);
    server_info.sin_addr.s_addr = inet_addr(IP);

    // 发起链接
    while (count--) {
        if (connect(g_socket_fd, (struct sockaddr *)&server_info, sizeof(server_info)) == -1) {
            perror("connect");
            sleep(1);
            continue;
        }
        // 连接成功
        g_maxfd = (g_maxfd < g_socket_fd) ? g_socket_fd : g_maxfd;
        FD_SET(g_socket_fd, &READSET);  // 添加到可读集合中

        // 定时信息上传
        alarm(5);     //  SIGALRM 信号
        signal(SIGALRM, send_info2server);

    }
    
    return 0;
}