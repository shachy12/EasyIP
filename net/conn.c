#include <stdio.h>
#include <string.h>
#include <net/device.h>
#include <net/conn.h>
#include <net/ipv4/udp.h>
#include <net/arp/arp.h>
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
    connection->device = device;
    connection->type = type;
    connection->state = CONN__IDLE;
    CYCLIC_BUFFER__init(&connection->window_cyclic_buffer,
                        connection->window,
                        sizeof(connection->window));
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

bool CONN__recvfrom(CONNECTION_t *self, uint8_t *buffer, uint16_t length, ENDPOINT_t *endpoint, size_t *out_length)
{
    bool rc = false;
    CONNECTION__payload_header_t payload_header;
    size_t read_length = 0;

    IF_FALSE_GOTO(EASY_IP__lock_mutex(&CONN__mutex_g), ErrorHandling);
    self->state = CONN__WAITING_FOR_NEW_DATA;
    IF_FALSE_GOTO(CYCLIC_BUFFER__is_empty(&self->window_cyclic_buffer), ReadFromBuffer);
    /* We don't have data in the window so we need to wait for more data */
    IF_FALSE_GOTO(EASY_IP__unlock_mutex(&CONN__mutex_g), ErrorHandling);
    IF_FALSE_GOTO(EASY_IP__wait_signal(&self->signal), ErrorHandling);
    IF_FALSE_GOTO(EASY_IP__lock_mutex(&CONN__mutex_g), ErrorHandling);

ReadFromBuffer:
    self->state = CONN__IDLE;
    read_length = CYCLIC_BUFFER__read(&self->window_cyclic_buffer, &payload_header, sizeof(payload_header));
    IF_FALSE_GOTO(read_length == sizeof(payload_header), UnlockMutexAndClearBuffer);

    CYCLIC_BUFFER__pop(&self->window_cyclic_buffer, sizeof(payload_header));
    *out_length = CYCLIC_BUFFER__read(&self->window_cyclic_buffer, buffer, MIN(length, payload_header.size));
    CYCLIC_BUFFER__pop(&self->window_cyclic_buffer, *out_length);
    IF_FALSE_GOTO(EASY_IP__unlock_mutex(&CONN__mutex_g), ErrorHandling);

    rc = true;
    ON_NULL_GOTO(endpoint, Exit);
    memcpy(endpoint, &payload_header.endpoint, sizeof(*endpoint));
    goto Exit;
UnlockMutexAndClearBuffer:
    /* Something strange happend, we need to clear the buffer */
    CYCLIC_BUFFER__clear(&self->window_cyclic_buffer);
    EASY_IP__unlock_mutex(&CONN__mutex_g);
ErrorHandling:
    rc = false;
Exit:
    return rc;
}

bool CONN__sendto(CONNECTION_t *self, uint8_t *buffer, uint16_t length, ENDPOINT_t *endpoint)
{
    bool rc = false;
    DEVICE_t *device = self->device;
    MAC_ADDRESS_t destination_mac = {0};
    IF_FALSE_GOTO(EASY_IP__lock_mutex(&CONN__mutex_g), ErrorHandling);
    IF_TRUE_GOTO(ARP_CACHE__get_mac(&device->arp_cache, endpoint->ip, destination_mac),
                 SendPacket);

    self->state = CONN__WAITING_FOR_MAC;
    memcpy(self->state_params.mac_request_ip, endpoint->ip, sizeof(self->state_params.mac_request_ip));
    IF_FALSE_GOTO(EASY_IP__unlock_mutex(&CONN__mutex_g), ErrorHandling);
    ARP__request_mac(self, endpoint->ip);
    IF_FALSE_GOTO(EASY_IP__wait_signal(&self->signal), ErrorHandling);
    IF_FALSE_GOTO(EASY_IP__lock_mutex(&CONN__mutex_g), ErrorHandling);
    IF_FALSE_GOTO(ARP_CACHE__get_mac(&device->arp_cache, endpoint->ip, destination_mac),
                 ErrorHandling);

SendPacket:
    IF_FALSE_GOTO(EASY_IP__unlock_mutex(&CONN__mutex_g), ErrorHandling);
    UDP__send_packet(self, destination_mac, endpoint->ip, endpoint->port, buffer, length);
    rc = true;
    goto Exit;
ErrorHandling:
    rc = false;
Exit:
    return rc;
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

bool CONN__handle_arp_response(IP_ADDRESS_t ip)
{
    bool rc = false;
    LINKED_LIST__node_t *node = NULL;
    CONNECTION_t *connection = NULL;
    IF_FALSE_GOTO(EASY_IP__lock_mutex(&CONN__mutex_g), Exit);
    printf("Searching for connection\n");
    LINKED_LIST_FOREACH_NODE(&CONN__linked_list_g, node) {
        connection = (CONNECTION_t *)node;
        if ((CONN__WAITING_FOR_MAC == connection->state) &&
            (0 == memcmp(connection->state_params.mac_request_ip, ip, sizeof(connection->state_params.mac_request_ip)))) {
            printf("Connection found!\n");
            EASY_IP__post_signal(&connection->signal);
        }
    }

UnlockMutex:
    IF_FALSE_GOTO(EASY_IP__unlock_mutex(&CONN__mutex_g),
                  ErrorHandling);
    rc = true;
    goto Exit;
ErrorHandling:
    rc = false;
Exit:
    return rc;
}

bool CONN__push_data_to_window(CONNECTION_t *self,
                               IP_ADDRESS_t source_ip,
                               uint16_t source_port,
                               uint8_t *buffer,
                               uint16_t length)
{
    bool rc = false;
    CONNECTION__payload_header_t payload_header = {0};
    memcpy(payload_header.endpoint.ip, source_ip, sizeof(payload_header.endpoint.ip));
    payload_header.endpoint.port = source_port;
    payload_header.size = length;

    IF_FALSE_GOTO(EASY_IP__lock_mutex(&CONN__mutex_g), ErrorHandling);
    IF_FALSE_GOTO(CYCLIC_BUFFER__validate_enough_space(&self->window_cyclic_buffer,
                                                       length + sizeof(payload_header)),
                  UnlockMutex);
    IF_FALSE_GOTO(CYCLIC_BUFFER__write(&self->window_cyclic_buffer,
                                       (uint8_t *)&payload_header,
                                       sizeof(payload_header)),
                  UnlockMutex);
    IF_FALSE_GOTO(CYCLIC_BUFFER__write(&self->window_cyclic_buffer, buffer, length), UnlockMutex);
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
