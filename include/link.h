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
#include <player.h>



typedef struct Node {

    char music_name[64];
    struct Node *next;
    struct Node *prior;
} Node;


int link_init(void);
void create_link(const char *s);
int insert_link(const char *music_name);
void traverse_link();
void find_next_music(char *cur, int mode, char *next_music);

#endif