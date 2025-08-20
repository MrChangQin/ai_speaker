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

#ifdef ARM
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

    if (init_serial() == -1) {
        perror("init serial");
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
#endif // DEBUG

    return 0;
}

void start_buzzer() {
    int freq = 1000;
    int ret = ioctl(g_buzzer_fd, 1, freq);
    if (ret == -1) {
        perror("ioctl buzzer");
        return;
    }

    usleep(500000);
    
    ret = ioctl(g_buzzer_fd, 0);
    if (ret == -1) { 
        perror("ioctl buzzer");
    }
}

int get_key_id() {
    char buttons[6] = {'0', '0', '0', '0', '0', '0'};
    char cur_buttons[6] = {0};

    ssize_t size = read(g_buttons_fd, cur_buttons, 6);
    if (size == -1) {
        perror("read buttons");
        return -1;
    }

    int i;
    for (i = 0; i < 6; i++) {
        if (buttons[i] != cur_buttons[i]) {
            return i + 1;
        }
    }
    return -1;
}

int init_serial() {

    struct termios TtyAttr;
    memset(&TtyAttr, 0, sizeof(struct termios));
    TtyAttr.c_iflag = IGNPAR;
    TtyAttr.c_cflag = B115200 | HUPCL | CS8 | CREAD | CLOCAL;
    TtyAttr.c_cc[VMIN] = 1;

    if (tcsetattr(g_serial_fd, TCSANOW, &TtyAttr) == -1) {
        perror("tcsetattr");
        return -1;
    }
}