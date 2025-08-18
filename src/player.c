#include "player.h"


extern int g_mixer_fd;
extern Node *head;

int g_shmid = 0; // shared memory id
int g_start_flag = 0; // 播放开始
int g_suspend_flag = 0; // 播放暂停


int init_shm(void) {

    // init shared memory  (physical memory)
    g_shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | IPC_EXCL);
    if (g_shmid == -1) {
        perror("shmget");
        return -1;
    }

    // 映射
    void *addr = shmat(g_shmid, NULL, 0);
    if (addr == (void *)-1) {
        perror("shmat");
        return -1;
    }

    Shm shm;
    memset(&shm, 0, sizeof(Shm));
    shm.mode = SEQUENCE;
    memcpy(addr, &shm, sizeof(Shm)); // 映射到共享内存中

    shmdt(addr); // 断开映射
}

// 
void get_shm(Shm *s) {
    void *addr = shmat(g_shmid, NULL, 0);
    if (addr == (void *)-1) {
        perror("shmat");
        return;
    }
    memcpy(s, addr, sizeof(Shm));
    shmdt(addr);
}

//
void get_volume(int *v) {
    if (ioctl(g_mixer_fd, SOUND_MIXER_READ_VOLUME, v) == -1) {
        perror("ioctl");
        return;
    }
    *v /= 257;
}

void get_music() {
    // 发送请求
    struct json_object *req = json_object_new_object();
    json_object_object_add(req, "cmd", json_object_new_string("get_music"));
    json_object_object_add(req, "singer", json_object_new_string("random"));

    socket_send_data(req);

    char msg[1024] = {0};
    socket_recv_data(msg);

    // 形成链表
    create_link(msg);

    // 释放json对象
    json_object_put(req);
}

void start_play() {
    if (g_start_flag == 1) {  // 播放已经开始
        return;
    }
    if (head->next == NULL) {  //播放列表为空
        return;
    }

    char music_name[32] = {0};
    strcpy(music_name, head->next->music_name);

    int dfl_volume = DFL_VOLUME;  // 设置默认音量
    dfl_volume *= 257;
    if (ioctl(g_mixer_fd, SOUND_MIXER_WRITE_VOLUME, &dfl_volume) == -1) {
        perror("ioctl");
        return -1;
    }

    g_start_flag = 1;
    play_music(music_name);
}

void grand_get_shm(Shm *shm) {
    // 获取共享内存
    int shmid = shmget(SHM_KEY, SHM_SIZE, 0);
    if (shmid == -1) {
        perror("grand shmget");
        return;
    }

    void *addr = shmat(shmid, NULL, 0);
    if (addr == (void *)-1) {
        perror("grand shmat");
        return;
    }

    memcpy(shm, &addr, sizeof(Shm)); // 映射到共享内存中
    shmdt(addr); // 断开映射
}
 
void grand_set_shm(Shm shm) { 
    
    // 更新共享内存
    int shmid = shmget(SHM_KEY, SHM_SIZE, 0);
    if (shmid == -1) {
        perror("grand shmget");
        return;
    }

    void *addr = shmat(shmid, NULL, 0);
    if (addr == (void *)-1) {
        perror("grand shmat");
        return;
    }

    shm.child_pid = getpid();
    shm.grand_pid = getpid();
    
    memcpy(addr, &shm, sizeof(Shm)); // 映射到共享内存中
    shmdt(addr); // 断开映射
}

void play_music(char *music_name) {
    pid_t child_pid = fork();
    if (child_pid == -1) { 
        perror("fork");
        return;
    }
    else if (child_pid == 0) { 
        close(0);                   // 关闭标准输入
        child_process(music_name);
    }
    else { 
        return;
    }
}

void child_process(const char *music_name) {
    while (g_start_flag)
    {   
        pid_t grand_pid = fork();
        if (grand_pid == -1) { 
            perror("fork");
            return;
        }
        else if (grand_pid == 0) { 
            Shm shm;
            memset(&shm, 0, sizeof(Shm));
            if (strlen(music_name) == 0) { // 第二次进入
                grand_get_shm(&shm);
                find_next_music(shm.cur_music, shm.mode, music_name);
                
                
            }
            char *arg[7] = {0};
            char music_path[128] = {0};
            strcpy(music_path, URL);
            strcat(music_path, music_name);

            arg[0] = "mplayer";
            arg[1] = music_path;
            arg[2] = "-slave";
            arg[3] = "-quiet";
            arg[4] = "-input";
            arg[5] = "file=./cmd_fifo";
            arg[6] = NULL;

            grand_get_shm(&shm);
            char *p = music_name; // “其他/music_name”
            while (*p != '/') {
                p++;
            }
            strcpy(shm.cur_music, p + 1); // “music_name”
            grand_set_shm(shm); // 更新共享内存

            execv("/usr/local/bin/mplayer", arg);
        }
        else { 
            memset(music_name, 0, sizeof(music_name))
            int status;
            wait(&status);
        }
    }
}