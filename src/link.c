#include "link.h"

extern int g_start_flag;
extern Node *head;

int link_init(void)
{

    head = (Node *)malloc(sizeof(Node) * 1);
    if (head == NULL) {
        return -1;
    }

    head->next = NULL;
    head->prior = NULL;

    return 0;
    
}

void create_link(const char *s) {
    struct json_object *obj = (struct json_object *)json_tokener_parse(s);
    if (obj == NULL) {
        printf("json_tokener_parse error\n");
        return;
    }

    struct json_object *array;
    array = (struct json_object *)json_object_object_get(obj, "music");
    int i = 0;
    for (i = 0; i < 5; i++) { 
        struct json_object *music = (struct json_object *)json_object_array_get_idx(array, i);
        if (insert_link((const char *)json_object_get_string(music)) == -1) {
            printf("insert_link error\n");
            return;
        }
        json_object_put(music);
    }

    json_object_put(obj);
    json_object_put(array);
}

int insert_link(const char *music_name) {
    Node *p = head;
    while (p->next != NULL) {
        p = p->next;
    }
    Node *new = (Node *)malloc(sizeof(Node));
    if (new == NULL) {
        perror("malloc");
        return -1;
    }
    strcpy(new->music_name, music_name);
    new->next = NULL;
    new->prior = p;
    p->next = new;
}

void traverse_link() {
    Node *p = head->next;
    while (p != NULL) {
        printf("%s\n", p->music_name);
        p = p->next;
    }
    printf("\n");
}

void clear_link() {
    Node *p = head->next;
    while (p != NULL) {
        head->next = p->next;
        free(p);
        p = head->next;
    }
}

void update_music() {
    g_start_flag = 0;

    // 回收子进程
    Shm shm;
    get_shm(&shm);
    int status;
    waitpid(shm.child_pid, &status, 0)

    char singer[128] = {0};
    get_singer(singer);

    clear_link();

    // 请求新的歌曲
    get_music(singer);

    // 开始播放
    start_play();
}

void get_singer(char *singer) {
    if (head->next == NULL)
        return;

    char *begin = head->next->music_name;
    char *p = begin;
    while (*p != '/')
    {
        p++;
    }
    strncpy(singer, begin, p - begin);
}

/*
返回
*/
int find_next_music(char *cur, int mode, char *next_music) {
    Node *p = head->next;
    while (p) {
        if (strstr(p->music_name, cur) != NULL) {
            break;
        }
        p = p->next;
    }
    // 找到当前音乐的链表节点p
    if (mode == CIRCLE) { 
        strcpy(next_music, p->music_name);
        return 0; // 循环播放
    }
    else if (mode == SEQUENCE) { 
        if (p->next != NULL) {
            strcpy(next_music, p->next->music_name);
            return 0; // 找到下一曲
        }
        else {
            return -1; // 未找到下一曲
        }
    }
}

void find_prior_music(char *cur, char *prior_music) {
    if (cur == NULL || prior_music == NULL) {
        return;
    }

    Node *p = head->next;
    if (strstr(p->music_name, cur)) {
        strcpy(prior_music, p->music_name);
        return;
    }

    p = p->next;
    while (p)
    {
        if (strstr(p->music_name, cur)) {
            strcpy(prior_music, p->prior->music_name);
            return;
        }
        p = p->next;
    }

    printf("未找到上一曲！\n");
}

