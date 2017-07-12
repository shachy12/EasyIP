#ifndef NET_IPV4_IP_H
#define NET_IPV4_IP_H

#include <net/device.h>

#define IP_HEADER_SIZE_WITHOUT_OPTIONS (20)
#define IP_GET_HEADER_LENGTH(__ip) ((__ip->version_header_length & 0xf) * 4)
#define IP_VERSION_HEADER_LENGTH (0x45)

typedef enum {
    eip_ip_ICMP_PROTOCOL = 1,
    eip_ip_UDP_PROTOCOL = 17,
} eip_ip_protocol_t;

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

void eip_ip_handle_packet(DEVICE_t *device, uint8_t *payload);

uint8_t *eip_ip_get_payload(IP_t *ip);

uint16_t eip_ip_fill(DEVICE_t *device, uint8_t *buffer, IP_ADDRESS_t destination_ip, uint8_t protocol, uint16_t payload_length);
#endif
