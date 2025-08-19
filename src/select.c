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
    case '2':
        stop_play();
        break;
    case '3':
        suspend_play();
        break;
    case '4':
        continue_play();
        break;
    case '5':
        next_play();
        break;
    case '6':
        prior_play();
        break;
    case '7':
        voice_up();
        break;
    case '8':
        voice_down();
        break;
    case '9':
        circle_play();
        break;
    case 'a':
        sequence_play();
        break;
    case 'b':
        singer_play("random");
        break;
    default:
        break;
    }
}

void select_read_button() {
    int key = get_key_id();

    switch (key)
    {
    case 1:
        start_play();
        break;
    case 2:
        stop_play();
        break;
    case 3:
        suspend_play();
        break;
    case 4:
        continue_play();
        break;
    case 5:
        next_play();
        break;
    case 6:
        prior_play();
        break;
    default:
        break;
    }
}

int parse_message(char *msg, char *cmd) {
    struct json_object *obj = (struct json_object *)json_tokener_parse(msg);
    if (obj == NULL) {
        printf("json_tokener_parse error\n");
        return -1;
    }

    struct json_object *value;
    value = (struct json_object *)json_object_object_get(obj, "cmd");
    if (value == NULL) {
        printf("json_object_object_get error\n");
        return -1;
    }
    strcpy(cmd, (const char *)json_object_get_string(value));
}

void select_read_socket() {

    char buf[1024] = {0};
    char cmd[128] = {0};

    socket_recv_data(buf);
    if (parse_message(buf, cmd) == -1) {  // json数据解析
        printf("parse message error\n");
    }
    else {
        printf("cmd = %s\n", cmd);
    }
}

void select_read_serial() {
    char ch;
    if (read(g_serial_fd, &ch, 1)) {
        perror("read serial error");
        return;
    }
    switch (ch)
    {
    case 1:
        start_play();
        break;
    case 2:
        stop_play();
        break;
    case 3:
        suspend_play();
        break;
    case 4:
        continue_play();
        break;
    case 5:
        prior_play();
        break;
    case 6:
        next_play();
        break;
    case 7:
        voice_up();
        break;
    case 8:
        voice_down();
        break;
    case 9:
        circle_play();
        break;
    case 0x0a:
        sequence_play();
        break;
    case 0x0b:
        singer_play("周杰伦");
        break;
    case 0x0c:
        singer_play("许嵩");
        break;
    case 0x0d:
        singer_play("陈奕迅");
        break;
    case 0x0e:
        singer_play("五月天");
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
            select_read_button();
        }
        else if (FD_ISSET(g_socket_fd, &tmpset)) {  // socket
            select_read_socket();
        }
        else if (FD_ISSET(g_serial_fd, &tmpset)) {  // 语音（语音模块-串口-开发板）
            select_read_serial();
        }
    }
}
