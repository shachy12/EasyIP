#ifndef NET_IPV4_IP_H
#define NET_IPV4_IP_H

#include <net/device.h>

#define IP__HEADER_SIZE_WITHOUT_OPTIONS (20)
#define IP__GET_HEADER_LENGTH(__ip) ((__ip->version_header_length & 0xf) * 4)
#define IP__VERSION_HEADER_LENGTH (0x45)

typedef enum {
    IP__ICMP_PROTOCOL = 1,
} IP__protocol_t;

typedef struct {
    uint8_t version_header_length;
    uint8_t dscp_ecn;
    uint16_t total_length;
    uint16_t identification;
    uint16_t flags_fragment_offset;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t header_checksum;
    IP_ADDRESS_t source_ip;
    IP_ADDRESS_t destination_ip;
    uint8_t payload[];
} IP_t;

void IP__handle_packet(DEVICE_t *device, uint8_t *payload);

uint8_t *IP__get_payload(IP_t *ip);

uint16_t IP__fill(DEVICE_t *device, uint8_t *buffer, IP_ADDRESS_t destination_ip, uint8_t protocol, uint16_t payload_length);
#endif
