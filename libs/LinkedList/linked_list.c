#include <stdint.h>
#include <string.h>
#include "linked_list.h"
#include <libs/Errors/errors.h>

void linked_list_init(LINKED_LIST_t *list)
{
    list->head = NULL;
}

void linked_list_add(LINKED_LIST_t *list, void *node_p)
{
    linked_list_node_t *node = (linked_list_node_t *)node_p;

    node->parent = list;
    node->prev = NULL;
    node->next = list->head;
    ON_NULL_GOTO(list->head,Exit);

    list->head->prev = node;
Exit:
    list->head = node;
}

void *linked_list_pop(LINKED_LIST_t *list)
{
    linked_list_node_t *node = list->head;
    ON_NULL_GOTO(node, Exit);

    linked_list_remove(node);
Exit:
    return node;
}

void *linked_list_get_first(LINKED_LIST_t *list)
{
    return list->head;
}

void *linked_list_get_next(void *node_p)
{
    linked_list_node_t *node = (linked_list_node_t *)node_p;
    return node->next;
}


void linked_list_remove(void *node_p)
{
    linked_list_node_t *node = (linked_list_node_t *)node_p;
    LINKED_LIST_t *list = node->parent;

    list->head = node->next;
    ON_NULL_GOTO(list->head, Exit);

    list->head->prev = NULL;
Exit:
    return;
}
