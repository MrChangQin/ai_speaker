#include "select.h"


extern fd_set READSET;   
extern int g_maxfd;
extern int g_buttons_fd;
extern int g_serial_fd;
extern int g_socket_fd;


void init_select(){
    FD_ZERO(&READSET);  // 初始化监听集合
    FD_SET(STDIN_FILENO, &READSET); // 将标准输入添加到监听集合中
}

void menu() {
    printf("************************************************************\n");
    printf("                  1.开始播放             2.结束播放\n");
    printf("                  3.暂停播放             4.继续播放\n");
    printf("                  5.下一首               6.上一首\n");
    printf("                  7.增加音量             8.减小音量\n");
    printf("                  9.单曲循环             a.顺序播放\n");
    printf("************************************************************\n");
}

void select_read_stdio() { 
    char ch;
    scanf("%c", &ch);

    switch (ch)
    {
    case '1':
        start_play();
        break;
    
    default:
        break;
    }
}

void m_select() {
    menu();
    fd_set tmpset;

    while (1) { 
        tmpset = READSET;
        int ret = select(g_maxfd + 1, &tmpset, NULL, NULL, NULL);
        if (ret = -1 && errno == EINTR) {
            continue;   
        }
        else if (ret == -1 && errno != EINTR) { 
            perror("select");
            return;
        }

        if (FD_ISSET(STDIN_FILENO, &tmpset)) {  // 键盘
            select_read_stdio();
        }
        else if (FD_ISSET(g_buttons_fd, &tmpset)) {  // 按钮

        }
        else if (FD_ISSET(g_socket_fd, &tmpset)) {  // socket

        }
        else if (FD_ISSET(g_serial_fd, &tmpset)) {  // 语音

        }
    }

}
