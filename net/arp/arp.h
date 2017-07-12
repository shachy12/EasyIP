#ifndef NET_ARP_H
#define NET_ARP_H

#include <stdint.h>
#include <net/device.h>
#include <net/protocols.h>
#include <net/conn.h>

#define ARP_REQUEST_TARGET_MAC ((uint8_t *)("\x00\x00\x00\x00\x00\x00"))

typedef enum {
    ARP_REQUEST = 1,
    ARP_REPLY = 2
} eip_arp_opcode_t;

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

uint16_t eip_arp_fill(DEVICE_t *device, uint8_t *buffer, uint8_t opcode, MAC_ADDRESS_t target_mac, IP_ADDRESS_t target_ip);

void eip_arp_handle_request(DEVICE_t *device, ARP_t *arp);

void eip_arp_handle_packet(DEVICE_t *device, uint8_t *payload);

bool eip_arp_request_mac(CONNECTION_t *self, IP_ADDRESS_t destination_ip);
#endif
