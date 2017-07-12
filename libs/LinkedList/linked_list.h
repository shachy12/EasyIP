#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdint.h>

typedef struct LINKED_LIST_s LINKED_LIST_t;

typedef struct linked_list_node_s {
    struct linked_list_node_s *next;
    struct linked_list_node_s *prev;
    LINKED_LIST_t *parent;
} linked_list_node_t;

struct LINKED_LIST_s {
    linked_list_node_t *head;
};

#define LINKED_LIST_FIELD_NAME __LINKED_LIST_NODE__
#define LINKED_LIST_NODE linked_list_node_t __LINKED_LIST_NODE__

void linked_list_init(LINKED_LIST_t *list);
void linked_list_add(LINKED_LIST_t *list, void *node_p);
void linked_list_remove(void *node_p);
void *linked_list_pop(LINKED_LIST_t *list);
void *linked_list_get_next(void *node_p);
void *linked_list_get_first(LINKED_LIST_t *list);

#define LINKED_LIST_FOREACH_NODE(__list, __node) \
    for (__node = linked_list_get_first(__list); __node != NULL; __node = linked_list_get_next(__node))
#endif
