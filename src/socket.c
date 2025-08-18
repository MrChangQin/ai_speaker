#include "socket.h"


extern int g_maxfd;
extern fd_set READSET;
int g_socket_fd = 0;


// 定时发送json信息
void send_info2server(int sig) {
    struct json_object *SendObj = json_object_new_object();
    json_object_object_add(SendObj, "cmd", json_object_new_string("info"));
    // 获取当前播放的音乐
    Shm s;
    memset(&s, 0, sizeof(s));
    get_shm(&s);
    json_object_object_add(SendObj, "cur_music", json_object_new_string(s.cur_music));
    json_object_object_add(SendObj, "mode", json_object_new_int(s.mode));
    // 获取当前系统音量
    int volume;
    get_volume(&volume);
    json_object_object_add(SendObj, "volume", json_object_new_int(volume));

    json_object_object_add(SendObj, "deviceid", json_object_new_string("0001"));

    // 发送到服务器
    socket_send_data(SendObj);
    // 释放
    json_object_put(SendObj);

    alarm(5);
}

void socket_send_data(struct json_object *obj) {
    char msg[1024] = {0};
    const char *s = json_object_to_json_string(obj);
    int len = strlen(s);

    memcpy(msg, &len, sizeof(int));
    memcpy(msg + sizeof(int), s, len);

    if (send(g_socket_fd, msg, len + 4, 0) == -1) {
        perror("send");
        return;
    }
}

void socket_recv_data(char *msg) {
    int len;
    ssize_t size  = 0;
    char buf[1024] = {0};
    
    while (1) {
        size += recv(g_socket_fd, buf + size, sizeof(int) - size, 0);
        if (size == sizeof(int)) break;
    }

    len = *(int *)buf;
    printf("--LENGTH : %d", len);
    size = 0;
    while (1) {
        size += recv(g_socket_fd, msg + size, len - size, 0);
        if (size >= len) break;
    }
    printf("--MSG : %s", msg);
}

int init_socket(void) {
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
        break;
    }
    return 0;
}
