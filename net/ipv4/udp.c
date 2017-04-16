#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <net/device.h>
#include <net/ether.h>
#include <net/ipv4/ip.h>
#include <net/ipv4/udp.h>
#include <net/utils.h>
#include <net/conn.h>
#include <libs/Errors/ERRORS.h>

uint16_t UDP__fill(DEVICE_t *device, uint8_t *buffer, uint16_t source_port, uint16_t destination_port, uint16_t payload_length)
{
    UDP_t *udp = (UDP_t*)buffer;
    udp->source_port = htons(source_port);
    udp->destination_port = htons(destination_port);
    udp->length = htons(payload_length + sizeof(*udp));
    udp->checksum = 0;
    return sizeof(*udp);
}

bool UDP__send_packet(CONNECTION_t *connection, IP_ADDRESS_t destination_ip, uint16_t destination_port, uint8_t *payload, uint16_t payload_size)
{
    bool rc = false;
    DEVICE_t *device = connection->device;
    uint8_t *buffer = (uint8_t *)device->output_packet;
    ARP_CACHE__entry_t *arp_entry = ARP_CACHE__get(&device->arp_cache, destination_ip);
    if (NULL == arp_entry) {
        printf("MAC not found!\n");
        goto ErrorHandling;
    }

    IF_FALSE_GOTO(EASY_IP__lock_mutex(&connection->device->mutex), ErrorHandling);
    buffer += ETHER__fill(device, buffer, arp_entry->mac, IPV4_PROTOCOL);
    buffer += IP__fill(device, buffer, destination_ip, IP__UDP_PROTOCOL, sizeof(UDP_t) + payload_size);
    buffer += UDP__fill(device, buffer, connection->source_port, destination_port, payload_size);
    memcpy(buffer, payload, payload_size);
    buffer += payload_size;
    UTILS__fill_checksums(device->output_packet);
    EASY_IP__write_packet(device->output_packet, buffer - device->output_packet);
    IF_FALSE_GOTO(EASY_IP__unlock_mutex(&connection->device->mutex), ErrorHandling);
    rc = true;
    goto Exit;
ErrorHandling:
    rc = false;
Exit:
    return rc;
}

void UDP__handle_packet(DEVICE_t *device, IP_t *ip)
{
    ETHER_t *ether = (ETHER_t *)device->input_packet;
    UDP_t *udp = (UDP_t *)IP__get_payload(ip);
    uint16_t payload_size = ntohs(udp->length) - sizeof(*udp);
    if (!EASY_IP__lock_mutex(&device->mutex)) {
        /* TODO: Add error handling */
        printf("Failed to lock mutex\n");
        goto Exit;
    }
    udp->payload[ntohs(udp->length) - sizeof(*udp)] = 0;
    if (!EASY_IP__unlock_mutex(&device->mutex)) {
        /* TODO: Add error handling */
        printf("Failed to unlock mutex\n");
        goto Exit;
    }

    CONNECTION_t *connection = CONN__get_udp_connection_by_port(ntohs(udp->destination_port));
    ON_NULL_GOTO(connection, Exit);

    CONN__push_data_to_window(connection, ip->source_ip,
                              ntohs(udp->source_port),
                              udp->payload,
                              ntohs(udp->length) - sizeof(*udp));
Exit:
    return;
}
