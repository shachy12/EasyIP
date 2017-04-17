#include <stdint.h>
#include <string.h>
#include <libs/Errors/ERRORS.h>
#include "RECORDS_ALLOCATOR.h"

void RECORDS_ALLOCATOR__init(RECORDS_ALLOCATOR_t *allocator, void *array, size_t record_size, size_t count)
{
    RECORDS_ALLOCATOR__node_t *current_record = NULL;
    RECORDS_ALLOCATOR__node_t *next_record = NULL;
    memset(allocator, 0, sizeof(*allocator));
    for (uint16_t i = 0; i < count; i++) {
        current_record = (RECORDS_ALLOCATOR__node_t *)((uint8_t *)(array) + (record_size * i));
        LINKED_LIST__add(&allocator->free_head, current_record);
    }
}

void *RECORDS_ALLOCATOR__allocate(RECORDS_ALLOCATOR_t *allocator)
{
    RECORDS_ALLOCATOR__node_t *node = LINKED_LIST__pop(&allocator->free_head);
    ON_NULL_GOTO(node, Exit);

    LINKED_LIST__add(&allocator->allocated_head, node);
Exit:
    return node;
}

void RECORDS_ALLOCATOR__free(void *node_p)
{
    RECORDS_ALLOCATOR__node_t *node = (RECORDS_ALLOCATOR__node_t *)node_p;
    LINKED_LIST__remove(node);
    LINKED_LIST__add(&node->allocator->free_head, node);
}
