#ifndef PLAYER_H
#define PLAYER_H



#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>


#define SEQUENCE        1             // 顺序播放
#define CIRCLE          2             // 单曲循环


#define SHM_KEY         1234         // 共享内存的key, 0x4D2
#define SHM_SIZE        4 * 1024     // 最小一页，4kb


// 共享内存数据 
typedef struct Shm {
    pid_t child_pid;                 // 子进程的pid
    pid_t grand_pid;                 // 孙进程的pid
    char cur_music[128];             // 当前播放的音乐
    int  mode;                       // 播放模式
} Shm;


int init_shm(void);

#endif
