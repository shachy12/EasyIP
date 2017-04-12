#ifndef NET_DEVICE_H
#define NET_DEVICE_H

#include <stdint.h>
#include <net/protocols.h>
#include <net/arp/arp_cache.h>
#include <net_config.h>

typedef struct {
    MAC_ADDRESS_t mac;
    IP_ADDRESS_t ip;
    IP_ADDRESS_t subnet_mask;
    IP_ADDRESS_t gateway;
    ARP_CACHE__table_t arp_cache;
    uint8_t output_packet[MTU];
    uint8_t input_packet[MTU];
} DEVICE_t;

void DEVICE__init(DEVICE_t *device,
                  MAC_ADDRESS_t mac,
                  IP_ADDRESS_t ip,
                  IP_ADDRESS_t subnet_mask,
                  IP_ADDRESS_t gateway);

void DEVICE__handle_packet(DEVICE_t *device, uint16_t packet_size);

void write_packet(uint8_t *packet, uint16_t length);

#endif /* NET_DEVICE_H */
