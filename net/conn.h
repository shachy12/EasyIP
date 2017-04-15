#ifndef NET_CONN_H
#define NET_CONN_H

typedef enum {
    CONN__EVENT_CONNECTED,
    CONN__EVENT_CONNECT_FAILED,
    CONN__EVENT_NEW_DATA,
    CONN__EVENT_DATA_SENT,
    CONN__EVENT_CONNECTION_CLOSED
} CONN__event_t;

typedef enum {
    CONN__ERROR_TCP_CONNECTION_REFUSED,
} CONN__error_t;

struct CONNECTION_t;
typedef struct CONNECTION_t CONNECTION_t;

typedef void (*CONN__connected_handler_t)(CONNECTION_t *self, DEVICE_t *device, CONN__event_t event);

struct CONNECTION_s {
    CONN__connected_handler_t connected_handler;
    CONN__error_t last_error;
};

#endif
