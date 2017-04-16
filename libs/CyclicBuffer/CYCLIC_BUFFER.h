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

void CYCLIC_BUFFER__init(CYCLIC_BUFFER_t *self, uint8_t *buffer, size_t buffer_size);
bool CYCLIC_BUFFER__write(CYCLIC_BUFFER_t *self, uint8_t *buffer, size_t length);
void CYCLIC_BUFFER__read(CYCLIC_BUFFER_t *self, uint8_t *buffer, size_t length, size_t *out_length);
void CYCLIC_BUFFER__pop(CYCLIC_BUFFER_t *self, uint8_t *buffer, size_t length, size_t *out_length);
#endif
