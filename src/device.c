#include "device.h"


int g_mixer_fd = 0;
int g_buttons_fd = 0;

int init_dirver(void) {

    // 初始化音频设备
    g_mixer_fd = open("/dev/mixer", O_RDWR);
    if (g_mixer_fd < 0) {
        perror("open");
        return -1;
    }
    
    // 初始化按钮设备
    g_buttons_fd = open("/dev/buttons", );
    if (g_buttons_fd < 0) {
        perror("open");
        return -1;
    }

    // 初始化按键
}