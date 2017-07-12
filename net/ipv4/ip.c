#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <net/device.h>
#include <net/ipv4/ip.h>
#include <net/ipv4/icmp.h>
#include <net/ipv4/udp.h>
#include <net/utils.h>
#include <net_config.h>

uint8_t *eip_ip_get_payload(IP_t *ip)
{
    uint8_t *buffer = (uint8_t *)ip;
    /* uint16_t header_size = ip->header_length * 4; */
    /* printf("Header Length: %d\n", ip->header_length); */
    return buffer + IP_GET_HEADER_LENGTH(ip);
}

uint16_t eip_ip_fill(DEVICE_t *device, uint8_t *buffer, IP_ADDRESS_t destination_ip, uint8_t protocol, uint16_t payload_length)
{
    IP_t *ip = (IP_t *)buffer;
    ip->version_header_length = IP_VERSION_HEADER_LENGTH;
    ip->dscp_ecn = 0;
    ip->total_length = htons(payload_length + IP_GET_HEADER_LENGTH(ip));
    ip->identification = 0;
    /* TODO: deal with framentations */
    ip->flags_fragment_offset = htons(0x4000);
    ip->ttl = EASY_IP_TTL;
    ip->protocol = protocol;
    ip->header_checksum = 0;
    memcpy(ip->source_ip, device->ip, sizeof(ip->source_ip));
    memcpy(ip->destination_ip, destination_ip, sizeof(ip->destination_ip));

    return IP_GET_HEADER_LENGTH(ip);
}

void eip_ip_handle_packet(DEVICE_t *device, uint8_t *payload)
{
    IP_t *ip = (IP_t *)payload;
    if (0 != memcmp(ip->destination_ip, device->ip, sizeof(ip->destination_ip))) {
        goto Exit;
    }

    if (ip->version_header_length > 0x45) {
        /* TODO: Handle options */
    }

#define IP_PROTOCOL_HANDLER(__protocol, __handler) \
    if (ip->protocol == __protocol) {\
        __handler(device, ip);\
    }
#include <net/ipv4/ip_protocol_handlers.hx>
#undef IP_PROTOCOL_HANDLER

Exit:
    return;
}
