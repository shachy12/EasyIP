#include <stdint.h>
#include <string.h>
#include "RECORDS_ALLOCATOR.h"

void RECORDS_ALLOCATOR__init(RECORDS_ALLOCATOR_t *allocator, void *array, size_t record_size, size_t count)
{
    RECORDS_ALLOCATOR__node_t *current_record = NULL;
    RECORDS_ALLOCATOR__node_t *next_record = NULL;
    memset(allocator, 0, sizeof(*allocator));
    for (uint16_t i = 0; i < count - 1; i++) {
        current_record = (RECORDS_ALLOCATOR__node_t *)((uint8_t *)(array) + (record_size * i));
        next_record = (RECORDS_ALLOCATOR__node_t *)((uint8_t *)(array) + (record_size * (i + 1)));
        current_record->next = next_record;
        next_record->prev = current_record;
        next_record->next = NULL;
    }
    allocator->free_head = (RECORDS_ALLOCATOR__node_t *)array;
}

RECORDS_ALLOCATOR__node_t *RECORDS_ALLOCATOR__allocate(RECORDS_ALLOCATOR_t *allocator)
{
    RECORDS_ALLOCATOR__node_t *node = allocator->free_head;

    if (NULL == node) {
        goto Exit;
    }

    allocator->free_head = node->next;
    allocator->free_head->prev = NULL;

    node->prev = NULL;
    if (NULL != allocator->allocated_head) {
        allocator->allocated_head->prev = node;
    }
    node->next = allocator->allocated_head;
    allocator->allocated_head = node;
Exit:
    return node;
}

void RECORDS_ALLOCATOR__free(void *node_p)
{
    RECORDS_ALLOCATOR__node_t *node = (RECORDS_ALLOCATOR__node_t *)node_p;
    RECORDS_ALLOCATOR_t *allocator = node->allocator;
    if (NULL != node->prev) {
        node->prev->next = node->next;
    }

    if (NULL != node->next) {
        node->next->prev = node->prev;
    }

    node->prev = NULL;
    node->next = allocator->free_head;
    if (NULL != allocator->free_head) {
        allocator->free_head->prev = node;
    }
    allocator->free_head = node;
}
