#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <net/device.h>
#include <net/ether.h>
#include <net/ipv4/ip.h>
#include <net/ipv4/udp.h>
#include <net/utils.h>

uint16_t UDP__fill(DEVICE_t *device, uint8_t *buffer, uint16_t source_port, uint16_t destination_port, uint16_t payload_length)
{
    UDP_t *udp = (UDP_t*)buffer;
    udp->source_port = htons(source_port);
    udp->destination_port = htons(destination_port);
    udp->length = htons(payload_length + sizeof(*udp));
    udp->checksum = 0;
    return sizeof(*udp);
}

void UDP__handle_packet(DEVICE_t *device, IP_t *ip)
{
    ETHER_t *ether = (ETHER_t *)device->input_packet;
    UDP_t *udp = (UDP_t *)IP__get_payload(ip);
    uint16_t payload_size = ntohs(udp->length) - sizeof(*udp);
    printf("UDP from %d.%d.%d.%d, src port: %d, dst port: %d\n",
           ip->source_ip[0],
           ip->source_ip[1],
           ip->source_ip[2],
           ip->source_ip[3],
           ntohs(udp->source_port),
           ntohs(udp->destination_port));
    if (!EASY_IP__lock_mutex(&device->mutex)) {
        /* TODO: Add error handling */
        printf("Failed to lock mutex\n");
        goto Exit;
    }
    udp->payload[ntohs(udp->length) - sizeof(*udp)] = 0;
    printf("Received Data: %s\n", udp->payload);
    uint8_t *buffer = (uint8_t *)device->output_packet;
    buffer += ETHER__fill(device, buffer, ether->source, IPV4_PROTOCOL);
    buffer += IP__fill(device, buffer, ip->source_ip, IP__UDP_PROTOCOL, ntohs(udp->length));
    buffer += UDP__fill(device, buffer, ntohs(udp->destination_port), ntohs(udp->source_port), payload_size);
    memcpy(buffer, udp->payload, payload_size);
    buffer += payload_size;
    UTILS__fill_checksums(device->output_packet);
    EASY_IP__write_packet(device->output_packet, buffer - device->output_packet);

    if (!EASY_IP__unlock_mutex(&device->mutex)) {
        /* TODO: Add error handling */
        printf("Failed to unlock mutex\n");
        goto Exit;
    }
Exit:
    return;
}
