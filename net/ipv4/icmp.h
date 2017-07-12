#ifndef NET_IPV4_ICMP_H
#define NET_IPV4_ICMP_H

#include <net/device.h>

typedef struct {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint8_t payload[];
} ICMP_t;

void eip_icmp_handle_packet(DEVICE_t *device, IP_t *ip);

#endif
