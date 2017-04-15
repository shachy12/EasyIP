#ifndef RECORDS_ALLOCATOR_H
#define RECORDS_ALLOCATOR_H

#include <stdint.h>

typedef struct RECORDS_ALLOCATOR_s RECORDS_ALLOCATOR_t;

typedef struct RECORDS_ALLOCATOR__node_s {
    struct RECORDS_ALLOCATOR__node_s *next;
    struct RECORDS_ALLOCATOR__node_s *prev;
    RECORDS_ALLOCATOR_t *allocator;
} RECORDS_ALLOCATOR__node_t;

struct RECORDS_ALLOCATOR_s {
    RECORDS_ALLOCATOR__node_t *allocated_head;
    RECORDS_ALLOCATOR__node_t *free_head;
};

#define RECORDS_ALLOCATOR_FIELD_NAME __RECORD_ALLOCATOR_NODE__
#define RECORDS_ALLOCATOR_NODE RECORDS_ALLOCATOR__node_t __RECORD_ALLOCATOR_NODE__

#define RECORDS_ALLOCATOR_INIT(__allocator, __array) \
    RECORD_ALLOCATOR__init(__allocator, __array, sizeof(__array[0]), sizeof(__array) / sizeof(__array[0]))

RECORDS_ALLOCATOR__node_t *RECORDS_ALLOCATOR__allocate(RECORDS_ALLOCATOR_t *allocator);
void RECORDS_ALLOCATOR__free(void *node_p);
#endif
