#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/soundcard.h>


int main() {
    int fd = open("/dev/mixer", O_RDWR);

    if (fd == -1) {
        perror("open");
        return -1;
    }

    // 读取当前音量
    int volume;
    if (ioctl(fd, SOUND_MIXER_READ_VOLUME, &volume) == -1) {
        perror("ioctl");
        return -1;
    }
    volume /= 257;
    printf("当前音量: %d\n", volume);

    // 设置音量
    int new_volume = 50;
    new_volume *= 257;
    if (ioctl(fd, SOUND_MIXER_WRITE_VOLUME, &new_volume) == -1) {
        perror("ioctl");
        return -1;
    }

    return 0;
}