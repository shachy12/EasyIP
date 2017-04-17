#ifndef RECORDS_ALLOCATOR_H
#define RECORDS_ALLOCATOR_H

#include <stdint.h>
#include <libs/LinkedList/LINKED_LIST.h>

typedef struct RECORDS_ALLOCATOR_s RECORDS_ALLOCATOR_t;

typedef struct {
    LINKED_LIST__node_t node;
    RECORDS_ALLOCATOR_t *allocator;
} RECORDS_ALLOCATOR__node_t;

struct RECORDS_ALLOCATOR_s {
    LINKED_LIST_t allocated_head;
    LINKED_LIST_t free_head;
};

#define RECORDS_ALLOCATOR_FIELD_NAME __RECORD_ALLOCATOR_NODE__
#define RECORDS_ALLOCATOR_NODE RECORDS_ALLOCATOR__node_t __RECORD_ALLOCATOR_NODE__

void RECORDS_ALLOCATOR__init(RECORDS_ALLOCATOR_t *allocator, void *array, size_t record_size, size_t count);
void *RECORDS_ALLOCATOR__allocate(RECORDS_ALLOCATOR_t *allocator);
void RECORDS_ALLOCATOR__free(void *node_p);

#define RECORDS_ALLOCATOR_INIT(__allocator, __array) \
    RECORDS_ALLOCATOR__init(__allocator, __array, sizeof(__array[0]), sizeof(__array) / sizeof(__array[0]))

#define RECORDS_ALLOCATOR_FOREACH_NODE(__allocator, __record) \
    LINKED_LIST_FOREACH_NODE(&(__allocator)->allocated_head, __record)
#endif
