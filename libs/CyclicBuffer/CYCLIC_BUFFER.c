#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "CYCLIC_BUFFER.h"

void CYCLIC_BUFFER__init(CYCLIC_BUFFER_t *self, void *buffer, size_t buffer_size)
{
    self->buffer = (uint8_t *)buffer;
    self->buffer_size = buffer_size;
    self->start_index = 0;
    self->length = 0;
}

bool CYCLIC_BUFFER__write(CYCLIC_BUFFER_t *self, void *buffer_p, size_t length)
{
    bool rc = false;
    size_t bytes_written = 0;
    size_t current_byte_index = 0;
    uint8_t *buffer = (uint8_t *)buffer_p;

    if (!CYCLIC_BUFFER__validate_enough_space(self, length)) {
        rc = false;
        goto Exit;
    }

    while (bytes_written != length) {
        current_byte_index = (self->start_index + self->length + bytes_written) % self->buffer_size;
        self->buffer[current_byte_index] = buffer[bytes_written];
        bytes_written++;
    }
    self->length += bytes_written;

    rc = true;
Exit:
    return rc;
}

size_t CYCLIC_BUFFER__read(CYCLIC_BUFFER_t *self, void *buffer_p, size_t length)
{
    bool rc = false;
    size_t bytes_read = 0;
    size_t current_byte_index = 0;
    uint8_t *buffer = (uint8_t *)buffer_p;

    if (self->length < length) {
        length = self->length;
    }

    while (bytes_read != length) {
        current_byte_index = (self->start_index + bytes_read) % self->buffer_size;
        buffer[bytes_read] = self->buffer[current_byte_index];
        bytes_read++;
    }

    return bytes_read;
}

void CYCLIC_BUFFER__pop(CYCLIC_BUFFER_t *self, size_t length)
{
    if (length > self->length) {
        /* If length is bigger than current length we want to clear the whole buffer */
        length = self->length;
    }
    self->start_index += length;
    self->length -= length;
}

bool CYCLIC_BUFFER__validate_enough_space(CYCLIC_BUFFER_t *self, size_t length)
{
    return self->buffer_size - self->length >= length;
}

bool CYCLIC_BUFFER__is_empty(CYCLIC_BUFFER_t *self)
{
    return self->length == 0;
}

void CYCLIC_BUFFER__clear(CYCLIC_BUFFER_t *self)
{
    self->start_index = 0;
    self->length = 0;
}
