#include "socket.h"


#define DEVICE_ID   "0001"   // 每个音箱的硬编码MAC

extern int g_maxfd;
extern fd_set READSET;
extern Node *head; // 音乐链表头
extern int g_start_flag;
extern int g_suspend_flag;

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
    json_object_object_add(SendObj, "deviceid", json_object_new_string(DEVICE_ID));

    if (g_start_flag == 0) {  // 未开始播放
        json_object_object_add(SendObj, "status", json_object_new_string("stop"));
    }
    else if (g_start_flag == 1 && g_suspend_flag == 1) { // 已经开始播放但是暂停
        json_object_object_add(SendObj, "status", json_object_new_string("suspend"));
    }
    else if (g_start_flag == 1 && g_suspend_flag == 0) { // 已经开始播放且没有暂停
        json_object_object_add(SendObj, "status", json_object_new_string("start"));
    }

    // 发送到服务器
    socket_send_data(SendObj);
    // 释放
    json_object_put(SendObj);

    alarm(2);
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

void upload_music_list() {
    struct json_object *obj = json_object_new_object();
    json_object_object_add(obj, "cmd", json_object_new_string("upload_music"));

    // 获取音乐列表
    struct json_object *array = json_object_new_array();
    Node *current = head->next;
    while (current != NULL) {
        json_object_array_add(array, json_object_new_string(current->music_name));
        current = current->next;
    }
    json_object_object_add(obj, "music", array);

    // 发送到服务器
    socket_send_data(obj);
    // 释放
    json_object_put(obj);
    json_object_put(array);
}

void socket_start_play() {
    struct json_object *obj = json_object_new_object();
    json_object_object_add(obj, "cmd", json_object_new_string("app_start_reply"));

    if (start_play() == -1) {
        json_object_object_add(obj, "result", json_object_new_string("failure"));
    }
    else {
        json_object_object_add(obj, "result", json_object_new_string("success"));
    }

    socket_send_data(obj);
    json_object_put(obj);
}

void socket_stop_play() {
    struct json_object *obj = json_object_new_object();
    json_object_object_add(obj, "cmd", json_object_new_string("app_stop_reply"));
    stop_play();
    json_object_object_add(obj, "result", json_object_new_string("success"));

    socket_send_data(obj);
    json_object_put(obj);
}

void socket_suspend_play() {
    struct json_object *obj = json_object_new_object();
    json_object_object_add(obj, "cmd", json_object_new_string("app_suspend_reply"));
    suspend_play();
    json_object_object_add(obj, "result", json_object_new_string("success"));

    socket_send_data(obj);
    json_object_put(obj);
}

void socket_continue_play() {
    struct json_object *obj = json_object_new_object();
    json_object_object_add(obj, "cmd", json_object_new_string("app_continue_reply"));
    continue_play();
    json_object_object_add(obj, "result", json_object_new_string("success"));

    socket_send_data(obj);
    json_object_put(obj);
}

void socket_prior_play() {
    struct json_object *obj = json_object_new_object();
    json_object_object_add(obj, "cmd", json_object_new_string("app_prior_reply"));
    prior_play();  // prior_play在主进程执行，更新了共享内存
    json_object_object_add(obj, "result", json_object_new_string("success"));

    Shm shm;  // 获取当前音乐
    get_shm(&shm);
    json_object_object_add(obj, "music", json_object_new_string(shm.cur_music));

    socket_send_data(obj);
    json_object_put(obj);
}

void socket_next_play() {
    struct json_object *obj = json_object_new_object();
    json_object_object_add(obj, "cmd", json_object_new_string("app_next_reply"));
    next_play();  // next_play也在主进程执行，更新了共享内存
    json_object_object_add(obj, "result", json_object_new_string("success"));

    Shm shm;  // 获取当前音乐
    get_shm(&shm);
    json_object_object_add(obj, "music", json_object_new_string(shm.cur_music));

    socket_send_data(obj);
    json_object_put(obj);
}

void socket_voice_up() {
    struct json_object *obj = json_object_new_object();
    json_object_object_add(obj, "cmd", json_object_new_string("app_voice_up_reply"));
    voice_up();
    json_object_object_add(obj, "result", json_object_new_string("success"));

    int volume;
    get_volume(&volume);
    json_object_object_add(obj, "voice", json_object_new_int(volume));

    socket_send_data(obj);
    json_object_put(obj);
}

void socket_voice_down() {
    struct json_object *obj = json_object_new_object();
    json_object_object_add(obj, "cmd", json_object_new_string("app_voice_down_reply"));
    voice_down();
    json_object_object_add(obj, "result", json_object_new_string("success"));

    int volume;
    get_volume(&volume);
    json_object_object_add(obj, "voice", json_object_new_int(volume));

    socket_send_data(obj);
    json_object_put(obj);
}

void socket_circle_play() {
    struct json_object *obj = json_object_new_object();
    json_object_object_add(obj, "cmd", json_object_new_string("app_circle_reply"));
    circle_play();
    json_object_object_add(obj, "result", json_object_new_string("success"));

    socket_send_data(obj);
    json_object_put(obj);
}

void socket_sequence_play() {
    struct json_object *obj = json_object_new_object();
    json_object_object_add(obj, "cmd", json_object_new_string("app_sequence_reply"));
    sequence_play();
    json_object_object_add(obj, "result", json_object_new_string("success"));

    socket_send_data(obj);
    json_object_put(obj);
}

int init_socket() {
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

#ifdef ARM
        // 蜂鸣器0.5秒提示
        start_buzzer();
#endif

        // 连接成功
        g_maxfd = (g_maxfd < g_socket_fd) ? g_socket_fd : g_maxfd;
        FD_SET(g_socket_fd, &READSET);  // 添加到可读集合中
        // 定时信息上传
        // alarm(2);     //  SIGALRM 信号
        signal(SIGALRM, send_info2server);
        send_info2server(SIGALRM);
        break;
    }
    return 0;
}
