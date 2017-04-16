#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "CYCLIC_BUFFER.h"

void CYCLIC_BUFFER__init(CYCLIC_BUFFER_t *self, uint8_t *buffer, size_t buffer_size)
{
    self->buffer = buffer;
    self->buffer_size = buffer_size;
    self->start_index = 0;
    self->length = 0;
}

bool CYCLIC_BUFFER__write(CYCLIC_BUFFER_t *self, uint8_t *buffer, size_t length)
{
    bool rc = false;
    size_t bytes_written = 0;
    size_t current_byte_index = 0;

    if (self->buffer_size - self->length < length) {
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

void CYCLIC_BUFFER__read(CYCLIC_BUFFER_t *self, uint8_t *buffer, size_t length, size_t *out_length)
{
    bool rc = false;
    size_t bytes_read = 0;
    size_t current_byte_index = 0;

    if (self->length < length) {
        length = self->length;
    }

    while (bytes_read != length) {
        current_byte_index = (self->start_index + bytes_read) % self->buffer_size;
        buffer[bytes_read] = self->buffer[current_byte_index];
        bytes_read++;
    }

    *out_length = bytes_read;
}

void CYCLIC_BUFFER__pop(CYCLIC_BUFFER_t *self, uint8_t *buffer, size_t length, size_t *out_length)
{
    CYCLIC_BUFFER__read(self, buffer, length, out_length);
    self->start_index += *out_length;
    self->length -= *out_length;
}

void CYCLIC_BUFFER__clear(CYCLIC_BUFFER_t *self)
{
    self->start_index = 0;
    self->length = 0;
}
