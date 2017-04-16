#include <stdint.h>
#include <string.h>
#include "LINKED_LIST.h"

void LINKED_LIST__init(LINKED_LIST_t *list)
{
    list->head = NULL;
}

void LINKED_LIST__add(LINKED_LIST_t *list, void *node_p)
{
    LINKED_LIST__node_t *node = (LINKED_LIST__node_t *)node_p;

    node->prev = NULL;
    node->next = list->head;
    if (NULL != list->head) {
        list->head->prev = node;
    }
Exit:
    list->head = node;
}

void LINKED_LIST__remove(void *node_p)
{
    LINKED_LIST__node_t *node = (LINKED_LIST__node_t *)node_p;
    LINKED_LIST_t *list = node->parent;

    list->head = node->next;
    if (NULL != list->head) {
        list->head->prev = NULL;
    }
}
