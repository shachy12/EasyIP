#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <net/device.h>
#include <net/conn.h>
#include <libs/Errors/ERRORS.h>

#define UDP_SERVER__LISTENING_PORT (1234)

void *UDP_SERVER__thread(void *arg)
{
    DEVICE_t *device = (DEVICE_t *)arg;
    CONNECTION_t udp_server;
    uint8_t buffer[100] = {0};
    uint16_t length = 0;

    IF_FALSE_GOTO(CONN__create_socket(device, &udp_server, CONN__UDP), Exit);
    IF_FALSE_GOTO(CONN__bind(&udp_server, UDP_SERVER__LISTENING_PORT), CloseConnection);
    while (true) {
        IF_FALSE_GOTO(CONN__recvfrom(&udp_server, buffer, sizeof(buffer) - 1, NULL, &length),
                      CloseConnection);
        buffer[length] = 0;
        printf("Recevied %d bytes from socket!!!! %s\n", length, buffer);
    }
CloseConnection:
Exit:
    return NULL;
}
