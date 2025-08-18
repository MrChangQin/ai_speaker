#include "link.h"


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

void find_next_music(char *cur, int mode, char *next_music) {
    Node *p = head->next;
    while (p) {
        if (strstr(p->music_name, cur) == NULL) {
            p = p->next;
        }
    }
    // 找到当前音乐的链表节点p
    if (mode == CIRCLE) { 
        strcpy(next_music, p->music_name);
    }
    else if (mode == SEQUENCE) { 
        p = p->next;
        if (p == NULL) {
            strcpy(next_music, head->next->music_name);
        }
        else {
            strcpy(next_music, p->music_name);
        }
    }
}