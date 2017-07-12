#include <stdio.h>
#include <stdint.h>
#include <net/utils.h>
#include <net/protocols.h>
#include <net/ipv4/ip.h>
#include <net/ipv4/icmp.h>

void eip_utils_fill_checksums(uint8_t *buffer)
{
    ETHER_t *ether = (ETHER_t *)buffer;
    if (IPV4_PROTOCOL == ntohs(ether->protocol)) {
        IP_t *ip = (IP_t *)ether->payload;
        uint16_t header_length = IP_GET_HEADER_LENGTH(ip);
        ip->header_checksum = htons(checksum16((uint8_t *)ip, header_length));
        printf("Filling IP checksum %x, ", ip->header_checksum);
        if (ip->protocol == eip_ip_ICMP_PROTOCOL) {
            ICMP_t *icmp = (ICMP_t *)ip->payload;
            icmp->checksum = htons(checksum16(ip->payload, ntohs(ip->total_length) - header_length));
            printf("Filling ICMP checksum %x", icmp->checksum);
        }
        printf("\n");
    }
}

uint16_t checksum16(uint8_t *buffer, uint16_t length)
{
    uint32_t checksum = 0;
    uint16_t i = 0;
    while (i + 1 < length) {
        checksum += (((uint16_t)(buffer[i]) << 8) | (buffer[i+1]));
        i += 2;
    }
    if (length % 2 != 0) {
        checksum += buffer[i];
    }

    while (checksum >> 16) {
        checksum = (checksum & 0xffff) + (checksum >> 16);
    }

    return (uint16_t)(~checksum);
}
