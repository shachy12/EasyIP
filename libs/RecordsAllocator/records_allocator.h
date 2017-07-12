#ifndef RECORDS_ALLOCATOR_H
#define RECORDS_ALLOCATOR_H

#include <stdint.h>
#include <libs/LinkedList/linked_list.h>

typedef struct RECORDS_ALLOCATOR_s RECORDS_ALLOCATOR_t;

typedef struct {
    linked_list_node_t node;
    RECORDS_ALLOCATOR_t *allocator;
} records_allocator_node_t;

struct RECORDS_ALLOCATOR_s {
    LINKED_LIST_t allocated_head;
    LINKED_LIST_t free_head;
};

#define RECORDS_ALLOCATOR_FIELD_NAME __RECORD_ALLOCATOR_NODE__
#define RECORDS_ALLOCATOR_NODE records_allocator_node_t __RECORD_ALLOCATOR_NODE__

void records_allocator_init(RECORDS_ALLOCATOR_t *allocator, void *array, size_t record_size, size_t count);
void *records_allocator_allocate(RECORDS_ALLOCATOR_t *allocator);
void records_allocator_free(void *node_p);

#define RECORDS_ALLOCATOR_INIT(__allocator, __array) \
    records_allocator_init(__allocator, __array, sizeof(__array[0]), sizeof(__array) / sizeof(__array[0]))

#define RECORDS_ALLOCATOR_FOREACH_NODE(__allocator, __record) \
    LINKED_LIST_FOREACH_NODE(&(__allocator)->allocated_head, __record)
#endif
