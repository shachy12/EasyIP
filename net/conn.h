#ifndef NET_CONN_H
#define NET_CONN_H

#include <libs/LinkedList/LINKED_LIST.h>
#include <libs/CyclicBuffer/CYCLIC_BUFFER.h>
#include <net_config.h>

typedef enum {
    CONN__IDLE,
    CONN__WAITING_FOR_CONNECT_SUCCESS,
    CONN__WAITING_FOR_NEW_DATA,
    CONN__WAITING_TO_SEND_DATA,
    CONN__WAITING_FOR_CLOSE_SUCCESS,
} CONN__state_t;

typedef enum {
    CONN__UDP_SEND_DATA,
    CONN__UDP_RECV_DATA,
    CONN__TCP_SEND_DATA,
    CONN__TCP_RECV_DATA
} CONN__action_t;

typedef enum {
    CONN__UDP,
    CONN__TCP,
} CONN__type_t;

typedef enum {
    CONN__ERROR_TCP_CONNECTION_REFUSED,
    CONN__ERROR_UDP_PORT_UNREACHABLE,
} CONN__error_t;

typedef bool (*CONN__send_t)(uint8_t *data, uint16_t length);

typedef struct {
    IP_ADDRESS_t ip;
    uint16_t port;
} ENDPOINT_t;

typedef struct {
    ENDPOINT_t endpoint;
    size_t size;
} CONNECTION__payload_header_t;

typedef struct {
    LINKED_LIST_NODE;
    DEVICE_t *device;
    CONN__type_t type;
    CONN__state_t state;
    uint16_t source_port;
    uint8_t window[EASY_IP_CONNECTION_WINDOW_SIZE];
    CYCLIC_BUFFER_t window_cyclic_buffer;
    EASY_IP_SIGNAL_TYPE signal;
    CONNECTION__payload_header_t current_payload_header;
} CONNECTION_t;

bool CONN__init(void);
bool CONN__create_socket(DEVICE_t *device, CONNECTION_t *connection, CONN__type_t type);
bool CONN__bind(CONNECTION_t *self, uint16_t port);
bool CONN__recvfrom(CONNECTION_t *self, uint8_t *buffer, uint16_t length, ENDPOINT_t *endpoint, size_t *out_length);
void CONN__sendto(CONNECTION_t *self, uint8_t *buffer, uint16_t length, ENDPOINT_t *endpoint);
CONNECTION_t *CONN__get_udp_connection_by_port(uint16_t source_port);
bool CONN__push_data_to_window(CONNECTION_t *self,
                               IP_ADDRESS_t source_ip,
                               uint16_t source_port,
                               uint8_t *buffer,
                               uint16_t length);

#endif
