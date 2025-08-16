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