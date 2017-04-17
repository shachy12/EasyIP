#ifndef NET_UDP_H
#define NET_UDP_H

#include <net/device.h>
#include <net/conn.h>
#include <net/ipv4/ip.h>

typedef struct {
    uint16_t source_port;
    uint16_t destination_port;
    uint16_t length;
    uint16_t checksum;
    uint8_t payload[];
} UDP_t;

void UDP__handle_packet(DEVICE_t *device, IP_t *ip);
uint16_t UDP__fill(DEVICE_t *device, uint8_t *buffer, uint16_t source_port, uint16_t destination_port, uint16_t payload_length);
bool UDP__send_packet(CONNECTION_t *connection,
                      MAC_ADDRESS_t destination_mac,
                      IP_ADDRESS_t destination_ip,
                      uint16_t destination_port,
                      uint8_t *payload,
                      uint16_t payload_size);
#endif
