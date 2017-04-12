#ifndef NET_ARP_H
#define NET_ARP_H

#include <stdint.h>
#include <net/device.h>
#include <net/protocols.h>

typedef enum {
    ARP_REQUEST = 1,
    ARP_REPLY = 2
} ARP__opcode_t;

typedef struct {
    uint16_t hw_type;
    uint16_t protocol_type;
    uint8_t hw_size;
    uint8_t protocol_size;
    uint16_t opcode;
    MAC_ADDRESS_t sender_mac;
    IP_ADDRESS_t sender_ip;
    MAC_ADDRESS_t target_mac;
    IP_ADDRESS_t target_ip;
} ARP_t;

uint16_t ARP__fill(DEVICE_t *device, uint8_t *buffer, uint8_t opcode, MAC_ADDRESS_t target_mac, IP_ADDRESS_t target_ip);

void ARP__handle_request(DEVICE_t *device, ARP_t *arp);

void ARP__handle_packet(DEVICE_t *device, uint8_t *payload);

#endif
