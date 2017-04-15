#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <net/device.h>
#include <net/ether.h>
#include <net/ipv4/ip.h>
#include <net/ipv4/icmp.h>
#include <net/utils.h>

uint16_t ICMP__fill(DEVICE_t *device, uint8_t *buffer, uint8_t type, uint8_t code)
{
    ICMP_t *icmp = (ICMP_t *)buffer;
    icmp->type = type;
    icmp->code = code;
    icmp->checksum = 0;
    return sizeof(*icmp);
}

void ICMP__handle_packet(DEVICE_t *device, IP_t *ip)
{
    ETHER_t *ether = (ETHER_t *)device->input_packet;
    ICMP_t *icmp = (ICMP_t *)IP__get_payload(ip);
    uint16_t payload_size = ntohs(ip->total_length) - IP__GET_HEADER_LENGTH(ip);
    printf("ICMP from %d.%d.%d.%d, type: %d, code: %d\n",
           ip->source_ip[0],
           ip->source_ip[1],
           ip->source_ip[2],
           ip->source_ip[3],
           icmp->type,
           icmp->code);

    if (icmp->type  == 8 && icmp->code == 0) {

        if (!EASY_IP__lock_mutex(&device->mutex)) {
            /* TODO: Add error handling */
            printf("Failed to lock mutex\n");
            goto Exit;
        }
        uint8_t *buffer = (uint8_t *)device->output_packet;
        buffer += ETHER__fill(device, buffer, ether->source, IPV4_PROTOCOL);
        buffer += IP__fill(device, buffer, ip->source_ip, IP__ICMP_PROTOCOL, payload_size);
        buffer += ICMP__fill(device, buffer, 0, 0);
        memcpy(buffer, icmp->payload, payload_size);
        buffer += payload_size;
        UTILS__fill_checksums(device->output_packet);
        EASY_IP__write_packet(device->output_packet, buffer - device->output_packet);
        if (!EASY_IP__unlock_mutex(&device->mutex)) {
            /* TODO: Add error handling */
            printf("Failed to unlock mutex\n");
            goto Exit;
        }
    }
Exit:
    return;
}
