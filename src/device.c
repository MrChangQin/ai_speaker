#include "device.h"


extern int g_maxfd;
extern fd_set READSET;

int g_mixer_fd = 0;
int g_buttons_fd = 0;
int g_serial_fd = 0;
int g_buzzer_fd = 0;


/* 初始化驱动*/
int init_dirver(void) {

    // 初始化音频设备
    g_mixer_fd = open("/dev/mixer", O_RDWR);
    if (g_mixer_fd ==-1 ) {
        perror("open mixer");
        return -1;
    }
    g_maxfd = (g_maxfd < g_mixer_fd) ? g_mixer_fd : g_maxfd;

    // 初始化按键设备
    g_buttons_fd = open("/dev/buttons", O_RDONLY);
    if (g_buttons_fd == -1) {
        perror("open buttons");
        return -1;
    }
    g_maxfd = (g_maxfd < g_buttons_fd) ? g_buttons_fd : g_maxfd;
    FD_SET(g_buttons_fd, &READSET);

    // 初始化串口
    g_serial_fd = open("/dev/ttySAC1", O_RDONLY);
    if (g_serial_fd == -1)
    {
        perror("open serial");
        return -1;
    }
    g_maxfd = (g_maxfd < g_serial_fd) ? g_serial_fd : g_maxfd;
    FD_SET(g_serial_fd, &READSET);
    
    // 初始化buzzer
    g_buzzer_fd = open("/dev/pwm", O_WRONLY);
    if (g_buzzer_fd == -1)
    {
        perror("open pwm");
        return -1;
    }
    g_maxfd = (g_maxfd < g_buzzer_fd) ? g_buzzer_fd : g_maxfd;
    FD_SET(g_buzzer_fd, &READSET);

    return 0;

}