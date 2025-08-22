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
    shm.parent_pid = getpid();
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

void set_shm(Shm s) {
    void *addr = shmat(g_shmid, NULL, 0);
    if (addr == (void *)-1) {
        perror("shmat");
        return;
    }
    memcpy(addr, &s, sizeof(Shm));
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

void get_music(const char *singer) {
    // 发送请求
    struct json_object *req = json_object_new_object();
    json_object_object_add(req, "cmd", json_object_new_string("get_music_list"));
    json_object_object_add(req, "singer", json_object_new_string(singer));

    socket_send_data(req);

    char msg[1024] = {0};
    socket_recv_data(msg);

    // 形成链表
    create_link(msg);

    // 上传音乐数据 （为了更新APP）
    upload_music_list();

    // 释放json对象
    json_object_put(req);
}

int start_play() {
    if (g_start_flag == 1) {  // 播放已经开始
        return -1;
    }
    if (head->next == NULL) {  //播放列表为空
        return -1;
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

    return 0;
}

void write_fifo(const char *cmd) { 
    int fd = open("cmd_fifo", O_WRONLY);
    if (fd == -1) { 
        perror("fifo open");
        return;
    }

    if (write(fd, cmd, strlen(cmd)) == -1) {
        perror("fifo write");
    }
    close(fd);
}

void stop_play() {

    if (g_start_flag == 0) {
        return;
    }

    // 通知子进程
    Shm shm;
    get_shm(&shm);
    kill(shm.child_pid, SIGUSR2);

    // 结束mplayer进程
    // write_fifo("stop\n");
    write_fifo("quit\n");

    // 回收子进程
    int status;
    waitpid(s.child_pid, &status, 0);

    g_start_flag = 0;
}

void suspend_play() {
    if (g_start_flag == 0 || g_suspend_flag == 1) {
        return;
    }
    write_fifo("pause\n");
    printf("暂停播放\n");
    g_suspend_flag = 1;
}

void continue_play() {
    if (g_start_flag == 0 || g_suspend_flag == 0) {
        return;
    }
    write_fifo("pause\n");
    printf("继续播放\n");
    g_suspend_flag = 0;
}

void next_play() {
    if (g_start_flag == 0) {
        return;
    }

    Shm shm;
    get_shm(&shm);
    char music_name[128] = {0};
    if (find_next_music(shm.cur_music, SEQUENCE, music_name) == -1) {
        stop_play();

        char singer[128] = {0};
        get_singer(singer);

        clear_link();
        get_music("random");

        sleep(1);// 稳定很多

        start_play();
        if (g_suspend_flag == 1) {
            g_suspend_flag == 0;
        }

        return;
    }

    char path[256] = {0};
    strcat(path, URL);
    strcat(path, music_name);

    char cmd[256] = {0};
    sprintf(cmd, "loadfile %s\n", path);
    write_fifo(cmd);

    int i;
    for (i = 0; i < sizeof(music_name); i++) {
        if (music_name[i] == '/')
        break;
    }
    strcpy(shm.cur_music, music_name + i + 1);
    set_shm(shm);

    if (g_suspend_flag == 1) {
        g_suspend_flag == 0;
    }
}

void prior_play() {
    if (g_start_flag == 0) {
        return;
    }

    Shm shm;
    get_shm(&shm);
    char music_name[128] = {0};
    find_prior_music(shm.cur_music, music_name);

    char path[256] = {0};
    strcat(path, URL);
    strcat(path, music_name);

    char cmd[256] = {0};
    sprintf(cmd, "loadfile %s\n", path);
    write_fifo(cmd);

    int i;
    for (i = 0; i < sizeof(music_name); i++) {
        if (music_name[i] == '/')
        break;
    }
    strcpy(shm.cur_music, music_name + i + 1);
    set_shm(shm);

    if (g_suspend_flag == 1) {
        g_suspend_flag == 0;
    }
}

void singer_play(const char *singer) {
    // 停止播放
    stop_play();
    // 清空链表
    clear_list();
    // 获取歌曲
    get_music(singer);
    // 开始播放
    start_play();
}

void voice_up() {
    int volume;
    if (ioctl(g_mixer_fd, SOUND_MIXER_READ_VOLUME, &volume) == -1) {
        perror("ioctl");
        return;
    }
    volume /= 257;

    if (volume <= 95) {
        volume += 5;
    }
    else if (volume > 95 && volume < 100) {
        volume = 100;
    }
    else if (volume == 100) {
        printf("音量已最大\n");
        return;
    }

    volume *= 257;
    if (ioctl(g_mixer_fd, SOUND_MIXER_WRITE_VOLUME, &volume) == -1) {
        perror("ioctl");
        return;
    }
    printf("增加音量:%d\n", volume / 257);
}

void voice_down() {
    int volume;
    if (ioctl(g_mixer_fd, SOUND_MIXER_READ_VOLUME, &volume) == -1) {
        perror("ioctl");
        return;
    }
    volume /= 257;

    if (volume >= 5) {
        volume -= 5;
    }
    else if (volume > 0 && volume < 5) {
        volume = 0;
    }
    else if (volume == 0) {
        printf("音量已最小\n");
        return;
    }

    volume *= 257;
    if (ioctl(g_mixer_fd, SOUND_MIXER_WRITE_VOLUME, &volume) == -1) {
        perror("ioctl");
        return;
    }
    printf("减小音量:%d\n", volume / 257);
}

void circle_play() {
    Shm shm;
    get_shm(&shm);
    s.mode = CIRCLE;
    set_shm(shm);
    printf("循环播放\n");
}

void sequence_play() {
    Shm shm;
    get_shm(&shm);
    s.mode = SEQUENCE;
    set_shm(shm);
    printf("顺序播放\n")
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

void child_quit() {
    g_start_flag = 0;
}

void play_music(char *music_name) {  // 创建子进程播放音乐
    pid_t child_pid = fork();
    if (child_pid == -1) { 
        perror("fork");
        return;
    }
    else if (child_pid == 0) { 
        close(0);                   // 关闭标准输入
        signal(SIGUSR2, child_quit);   // 监听信号
        child_process(music_name);
        exit(0);  // 退出子进程
    }
    else { 
        return;
    }
}

void child_process(const char *music_name) {  // 创建孙进程播放音乐
    while (g_start_flag)
    {   
        pid_t grand_pid = fork();
        if (grand_pid == -1) { 
            perror("fork");
            return;
        }
        else if (grand_pid == 0) { 
            close(0);   // 关闭标准输入
            Shm shm;
            memset(&shm, 0, sizeof(Shm));
            if (strlen(music_name) == 0) { // 第二次进入
                grand_get_shm(&shm);
                if (find_next_music(shm.cur_music, shm.mode, music_name) == -1) {
                    // 通知父进程和子进程
                    kill(shm.parent_pid, SIGUSR1); // 父进程执行 update_music
                    kill(shm.child_pid, SIGUSR2);  // 子进程修改 g_start_flag
                    usleep(100000); // 等待父进程和子进程执行完毕
                    exit(0);
                }
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

#ifdef ARM
            execv("/bin/mplayer", arg); // 开发板上的mplayer
#else
            execv("/usr/local/bin/mplayer", arg); // 虚拟机上的mplayer
#endif
        }
        else { 
            memset(music_name, 0, sizeof(music_name))
            int status;
            wait(&status);

            usleep(100000);
        }
    }
}