#include <stdint.h>
#include <string.h>
#include "LINKED_LIST.h"
#include <libs/Errors/ERRORS.h>

void LINKED_LIST__init(LINKED_LIST_t *list)
{
    list->head = NULL;
}

void LINKED_LIST__add(LINKED_LIST_t *list, void *node_p)
{
    LINKED_LIST__node_t *node = (LINKED_LIST__node_t *)node_p;

    node->parent = list;
    node->prev = NULL;
    node->next = list->head;
    ON_NULL_GOTO(list->head,Exit);

    list->head->prev = node;
Exit:
    list->head = node;
}

void *LINKED_LIST__pop(LINKED_LIST_t *list)
{
    LINKED_LIST__node_t *node = list->head;
    ON_NULL_GOTO(node, Exit);

    LINKED_LIST__remove(node);
Exit:
    return node;
}

void *LINKED_LIST__get_first(LINKED_LIST_t *list)
{
    return list->head;
}

void *LINKED_LIST__get_next(void *node_p)
{
    LINKED_LIST__node_t *node = (LINKED_LIST__node_t *)node_p;
    return node->next;
}


void LINKED_LIST__remove(void *node_p)
{
    LINKED_LIST__node_t *node = (LINKED_LIST__node_t *)node_p;
    LINKED_LIST_t *list = node->parent;

    list->head = node->next;
    ON_NULL_GOTO(list->head, Exit);

    list->head->prev = NULL;
Exit:
    return;
}
