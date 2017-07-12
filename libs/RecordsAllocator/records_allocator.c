#include <stdint.h>
#include <string.h>
#include <libs/Errors/errors.h>
#include "records_allocator.h"

void records_allocator_init(RECORDS_ALLOCATOR_t *allocator, void *array, size_t record_size, size_t count)
{
    records_allocator_node_t *current_record = NULL;
    records_allocator_node_t *next_record = NULL;
    memset(allocator, 0, sizeof(*allocator));
    for (uint16_t i = 0; i < count; i++) {
        current_record = (records_allocator_node_t *)((uint8_t *)(array) + (record_size * i));
        linked_list_add(&allocator->free_head, current_record);
    }
}

void *records_allocator_allocate(RECORDS_ALLOCATOR_t *allocator)
{
    records_allocator_node_t *node = linked_list_pop(&allocator->free_head);
    ON_NULL_GOTO(node, Exit);

    linked_list_add(&allocator->allocated_head, node);
Exit:
    return node;
}

void records_allocator_free(void *node_p)
{
    records_allocator_node_t *node = (records_allocator_node_t *)node_p;
    linked_list_remove(node);
    linked_list_add(&node->allocator->free_head, node);
}
