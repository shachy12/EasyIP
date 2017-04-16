#include <stdio.h>
#include <string.h>
#include <net/device.h>
#include <net/conn.h>
#include <net/utils.h>
#include <libs/Errors/ERRORS.h>

LINKED_LIST_t CONN__linked_list_g = {0};
EASY_IP_MUTEX_TYPE CONN__mutex_g;

bool CONN__init(void)
{
    LINKED_LIST__init(&CONN__linked_list_g);
    return EASY_IP__create_mutex(&CONN__mutex_g);
}

bool CONN__create_socket(DEVICE_t *device, CONNECTION_t *connection, CONN__type_t type)
{
    memset(connection, 0, sizeof(*connection));
    IF_FALSE_GOTO(EASY_IP__lock_mutex(&CONN__mutex_g), ErrorHandling);

    LINKED_LIST__add(&CONN__linked_list_g, connection);
    IF_FALSE_GOTO(EASY_IP__create_signal(&connection->signal),
                  Cleanup);
    connection->type = type;
    connection->device = device;
    printf("Created Socket\n");

    goto Exit;
Cleanup:
    LINKED_LIST__remove(connection);
ErrorHandling:
    /* Ignoring return value because we already failed so it doesn't matter */
    EASY_IP__unlock_mutex(&CONN__mutex_g);
    return false;
Exit:
    return EASY_IP__unlock_mutex(&CONN__mutex_g);
}

bool CONN__bind(CONNECTION_t *self, uint16_t port)
{
    IF_FALSE_GOTO(EASY_IP__lock_mutex(&CONN__mutex_g), ErrorHandling);
    self->source_port = port;
    goto Exit;
ErrorHandling:
    EASY_IP__unlock_mutex(&CONN__mutex_g);
    return false;
Exit:
    return EASY_IP__unlock_mutex(&CONN__mutex_g);
}

bool CONN__recvfrom(CONNECTION_t *self, uint8_t *buffer, uint16_t length, ENDPOINT_t *endpoint, uint16_t *out_length)
{
    IF_FALSE_GOTO(EASY_IP__lock_mutex(&CONN__mutex_g), ErrorHandling);
    self->state = CONN__WAITING_FOR_NEW_DATA;
    if (0 == self->used_window) {
        /* We don't have data in the window so we need to wait for more data */
        IF_FALSE_GOTO(EASY_IP__unlock_mutex(&CONN__mutex_g), ErrorHandling);
        IF_FALSE_GOTO(EASY_IP__wait_signal(&self->signal), ErrorHandling);
        IF_FALSE_GOTO(EASY_IP__lock_mutex(&CONN__mutex_g), ErrorHandling);
    }
    uint16_t fill_size = MIN(length, self->used_window);
    memcpy(buffer, self->window, fill_size);
    buffer += fill_size;
    length -= fill_size;
    self->used_window -= fill_size;
    memcpy(self->window, self->window + fill_size, self->used_window);
    IF_FALSE_GOTO(EASY_IP__unlock_mutex(&CONN__mutex_g), ErrorHandling);
    *out_length = fill_size;
    goto Exit;
ErrorHandling:
    return false;
Exit:
    return true;
}

uint16_t CONN__sendto(CONNECTION_t *self, uint8_t *buffer, uint16_t length, IP_ADDRESS_t destination_ip)
{

}

CONNECTION_t *CONN__get_udp_connection_by_port(uint16_t source_port)
{
    LINKED_LIST__node_t *node = NULL;
    CONNECTION_t *connection = NULL;
    IF_FALSE_GOTO(EASY_IP__lock_mutex(&CONN__mutex_g), Exit);
    LINKED_LIST_FOREACH_NODE(&CONN__linked_list_g, node) {
        connection = (CONNECTION_t *)node;
        /* If true -> connection found returning.. */
        IF_TRUE_GOTO(connection->source_port == source_port,
                    UnlockMutex);
    }
    connection = NULL;

UnlockMutex:
    if (!EASY_IP__unlock_mutex(&CONN__mutex_g)) {
        /* ERROR! Return NULL instead of the connection */
        connection = NULL;
    }
Exit:
    return connection;
}

bool CONN__push_data_to_window(CONNECTION_t *self, uint8_t *buffer, uint16_t length)
{
    bool rc = false;
    IF_FALSE_GOTO(EASY_IP__lock_mutex(&CONN__mutex_g), ErrorHandling);
    IF_FALSE_GOTO(length < sizeof(self->window) - self->used_window,
                  UnlockMutex);
    memcpy(self->window + self->used_window, buffer, length);
    self->used_window += length;
    if (self->state == CONN__WAITING_FOR_NEW_DATA) {
        IF_FALSE_GOTO(EASY_IP__unlock_mutex(&CONN__mutex_g), ErrorHandling);
        IF_FALSE_GOTO(EASY_IP__post_signal(&self->signal), ErrorHandling);
    }
    else {
        IF_FALSE_GOTO(EASY_IP__unlock_mutex(&CONN__mutex_g), ErrorHandling);
    }
    rc = true;
    goto Exit;
UnlockMutex:
    EASY_IP__unlock_mutex(&CONN__mutex_g);
ErrorHandling:
    rc = false;
Exit:
    return rc;
}
