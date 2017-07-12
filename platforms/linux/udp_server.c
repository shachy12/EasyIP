#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <net/device.h>
#include <net/conn.h>
#include <libs/Errors/errors.h>

#define udp_server_LISTENING_PORT (1234)

void *udp_server_thread(void *arg)
{
    DEVICE_t *device = (DEVICE_t *)arg;
    CONNECTION_t udp_server;
    uint8_t buffer[100] = {0};
    size_t length = 0;
    ENDPOINT_t client;

    IF_FALSE_GOTO(eip_conn_create_socket(device, &udp_server, eip_conn_UDP), Exit);
    IF_FALSE_GOTO(eip_conn_bind(&udp_server, udp_server_LISTENING_PORT), CloseConnection);

    while (true) {
        IF_FALSE_GOTO(eip_conn_recvfrom(&udp_server, buffer, sizeof(buffer) - 1, &client, &length),
                      CloseConnection);
        buffer[length] = 0;
        printf("%s", buffer);
        eip_conn_sendto(&udp_server, buffer, length, &client);

    }
CloseConnection:
Exit:
    printf("Closed thread\n");
    return NULL;
}
