#ifndef NET_CONN_H
#define NET_CONN_H

#include <libs/RecordsAllocator/RECORDS_ALLOCATOR.h>
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
    RECORDS_ALLOCATOR_NODE;
    CONN__type_t type;
    CONN__state_t state;
    CONN__action_t action;
    CONN__send_t send_function;
    CONN__error_t last_error;
    uint16_t source_port;
    uint8_t window[EASY_IP_CONNECTION_WINDOW_SIZE];
    EASY_IP_SIGNAL_TYPE signal;
} CONNECTION_t;

#endif
