#ifndef LINK_H
#define LINK_H

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>


typedef struct Node {

    char music_name[64];
    struct Node *next;
    struct Node *prior;
} Node;


int link_init(void);

#endif