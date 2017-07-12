#ifndef CYCLIC_BUFFER_H
#define CYCLIC_BUFFER_H

#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint8_t *buffer;
    size_t buffer_size;
    size_t start_index;
    size_t length;
} CYCLIC_BUFFER_t;

void cyclic_buffer_init(CYCLIC_BUFFER_t *self, void *buffer, size_t buffer_size);
bool cyclic_buffer_write(CYCLIC_BUFFER_t *self, void *buffer, size_t length);
size_t cyclic_buffer_read(CYCLIC_BUFFER_t *self, void *buffer, size_t length);
void cyclic_buffer_pop(CYCLIC_BUFFER_t *self, size_t length);
bool cyclic_buffer_validate_enough_space(CYCLIC_BUFFER_t *self, size_t length);
bool cyclic_buffer_is_empty(CYCLIC_BUFFER_t *self);
void cyclic_buffer_clear(CYCLIC_BUFFER_t *self);
#endif
