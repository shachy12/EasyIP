#ifndef NET_CONN_H
#define NET_CONN_H

#include <libs/LinkedList/linked_list.h>
#include <libs/CyclicBuffer/cyclic_buffer.h>
#include <net_config.h>

typedef enum {
    eip_conn_IDLE,
    eip_conn_WAITING_FOR_NEW_DATA,
    eip_conn_WAITING_FOR_MAC,
} eip_conn_state_t;

typedef enum {
    eip_conn_UDP_SEND_DATA,
    eip_conn_UDP_RECV_DATA,
    eip_conn_TCP_SEND_DATA,
    eip_conn_TCP_RECV_DATA
} eip_conn_action_t;

typedef enum {
    eip_conn_UDP,
    eip_conn_TCP,
} eip_conn_type_t;

typedef enum {
    eip_conn_ERROR_TCP_CONNECTION_REFUSED,
    eip_conn_ERROR_UDP_PORT_UNREACHABLE,
} eip_conn_error_t;

typedef bool (*eip_conn_send_t)(uint8_t *data, uint16_t length);

typedef struct {
    IP_ADDRESS_t ip;
    uint16_t port;
} ENDPOINT_t;

typedef struct {
    ENDPOINT_t endpoint;
    size_t size;
} eip_conn_payload_header_t;

typedef union {
    IP_ADDRESS_t mac_request_ip;
} eip_conn_state_params_t;

typedef struct {
    LINKED_LIST_NODE;
    DEVICE_t *device;
    eip_conn_type_t type;
    eip_conn_state_t state;
    eip_conn_state_params_t state_params;
    uint16_t source_port;
    uint8_t window[EASY_IP_CONNECTION_WINDOW_SIZE];
    CYCLIC_BUFFER_t window_cyclic_buffer;
    EASY_IP_SIGNAL_TYPE signal;
} CONNECTION_t;

bool eip_conn_init(void);
bool eip_conn_create_socket(DEVICE_t *device, CONNECTION_t *connection, eip_conn_type_t type);
bool eip_conn_bind(CONNECTION_t *self, uint16_t port);
bool eip_conn_recvfrom(CONNECTION_t *self, uint8_t *buffer, uint16_t length, ENDPOINT_t *endpoint, size_t *out_length);
bool eip_conn_sendto(CONNECTION_t *self, uint8_t *buffer, uint16_t length, ENDPOINT_t *endpoint);
CONNECTION_t *eip_conn_get_udp_connection_by_port(uint16_t source_port);
bool eip_conn_push_data_to_window(CONNECTION_t *self,
                               IP_ADDRESS_t source_ip,
                               uint16_t source_port,
                               uint8_t *buffer,
                               uint16_t length);
bool eip_conn_handle_arp_response(IP_ADDRESS_t ip);

#endif
