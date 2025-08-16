#include "player.h"


int g_shmid = 0; // shared memory id

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
    memset(&shm, 0, sizeof(shm));
    shm.mode = SEQUENCE;
    memcpy(addr, &shm, sizeof(shm)); // 映射到共享内存中

    shmdt(addr); // 断开映射
}