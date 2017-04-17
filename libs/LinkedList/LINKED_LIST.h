#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdint.h>

typedef struct LINKED_LIST_s LINKED_LIST_t;

typedef struct LINKED_LIST__node_s {
    struct LINKED_LIST__node_s *next;
    struct LINKED_LIST__node_s *prev;
    LINKED_LIST_t *parent;
} LINKED_LIST__node_t;

struct LINKED_LIST_s {
    LINKED_LIST__node_t *head;
};

#define LINKED_LIST_FIELD_NAME __LINKED_LIST_NODE__
#define LINKED_LIST_NODE LINKED_LIST__node_t __LINKED_LIST_NODE__

void LINKED_LIST__init(LINKED_LIST_t *list);
void LINKED_LIST__add(LINKED_LIST_t *list, void *node_p);
void LINKED_LIST__remove(void *node_p);
void *LINKED_LIST__pop(LINKED_LIST_t *list);
void *LINKED_LIST__get_next(void *node_p);
void *LINKED_LIST__get_first(LINKED_LIST_t *list);

#define LINKED_LIST_FOREACH_NODE(__list, __node) \
    for (__node = LINKED_LIST__get_first(__list); __node != NULL; __node = LINKED_LIST__get_next(__node))
#endif
