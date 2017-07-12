#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <net/device.h>
#include <net/ether.h>
#include <net/ipv4/ip.h>
#include <net/ipv4/udp.h>
#include <net/utils.h>
#include <net/conn.h>
#include <libs/Errors/errors.h>

uint16_t eip_udp_fill(DEVICE_t *device, uint8_t *buffer, uint16_t source_port, uint16_t destination_port, uint16_t payload_length)
{
    UDP_t *udp = (UDP_t*)buffer;
    udp->source_port = htons(source_port);
    udp->destination_port = htons(destination_port);
    udp->length = htons(payload_length + sizeof(*udp));
    udp->checksum = 0;
    return sizeof(*udp);
}

bool eip_udp_send_packet(CONNECTION_t *connection,
                      MAC_ADDRESS_t destination_mac,
                      IP_ADDRESS_t destination_ip,
                      uint16_t destination_port,
                      uint8_t *payload,
                      uint16_t payload_size)
{
    bool rc = false;
    DEVICE_t *device = connection->device;
    uint8_t *buffer = (uint8_t *)device->output_packet;

    IF_FALSE_GOTO(eip_lock_mutex(&connection->device->mutex), ErrorHandling);
    buffer += eip_ether_fill(device, buffer, destination_mac, IPV4_PROTOCOL);
    buffer += eip_ip_fill(device, buffer, destination_ip, eip_ip_UDP_PROTOCOL, sizeof(UDP_t) + payload_size);
    buffer += eip_udp_fill(device, buffer, connection->source_port, destination_port, payload_size);
    memcpy(buffer, payload, payload_size);
    buffer += payload_size;
    eip_utils_fill_checksums(device->output_packet);
    eip_write_packet(device->output_packet, buffer - device->output_packet);
    IF_FALSE_GOTO(eip_unlock_mutex(&connection->device->mutex), ErrorHandling);
    rc = true;
    goto Exit;
ErrorHandling:
    rc = false;
Exit:
    return rc;
}

void eip_udp_handle_packet(DEVICE_t *device, IP_t *ip)
{
    ETHER_t *ether = (ETHER_t *)device->input_packet;
    UDP_t *udp = (UDP_t *)eip_ip_get_payload(ip);
    uint16_t payload_size = ntohs(udp->length) - sizeof(*udp);
    if (!eip_lock_mutex(&device->mutex)) {
        /* TODO: Add error handling */
        printf("Failed to lock mutex\n");
        goto Exit;
    }
    udp->payload[ntohs(udp->length) - sizeof(*udp)] = 0;
    if (!eip_unlock_mutex(&device->mutex)) {
        /* TODO: Add error handling */
        printf("Failed to unlock mutex\n");
        goto Exit;
    }

    CONNECTION_t *connection = eip_conn_get_udp_connection_by_port(ntohs(udp->destination_port));
    ON_NULL_GOTO(connection, Exit);

    eip_conn_push_data_to_window(connection, ip->source_ip,
                              ntohs(udp->source_port),
                              udp->payload,
                              ntohs(udp->length) - sizeof(*udp));
Exit:
    return;
}
