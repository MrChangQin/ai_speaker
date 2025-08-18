#ifndef PLAYER_H
#define PLAYER_H


#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <linux/soundcard.h>
#include <socket.h>
#include <link.h>


#define SEQUENCE        1             // 顺序播放
#define CIRCLE          2             // 单曲循环

#define SHM_KEY         1234         // 共享内存的key, 0x4D2
#define SHM_SIZE        4 * 1024     // 最小一页，4kb

#define DFL_VOLUME      100;

#define URL "http://192.168.1.100:8000/music/"

// 共享内存数据 
typedef struct Shm {
    pid_t child_pid;                 // 子进程的pid
    pid_t grand_pid;                 // 孙进程的pid
    char cur_music[128];             // 当前播放的音乐
    int  mode;                       // 播放模式
} Shm;

struct


int init_shm(void);
void get_shm(Shm* s);
void get_volume(int *v);
void get_music();
void start_play();
void play_music(char *music_name);
void grand_get_shm(Shm *shm);
void grand_set_shm(Shm shm);
void child_process(const char *music_name);

#endif
