#include <stdio.h>
#include <string.h>
#include <net/device.h>
#include <net/protocols.h>
#include <net/ether.h>
#include <net/arp/arp.h>
#include <net/conn.h>
#include <net/utils.h>
#include <libs/Errors/ERRORS.h>

uint16_t ARP__fill(DEVICE_t *device, uint8_t *buffer, uint8_t opcode, MAC_ADDRESS_t target_mac, IP_ADDRESS_t target_ip)
{
    ARP_t *arp = (ARP_t *)buffer;
    arp->hw_type = htons(1);
    arp->protocol_type = htons(IPV4_PROTOCOL);
    arp->hw_size = sizeof(device->mac);
    arp->protocol_size = sizeof(device->ip);
    arp->opcode = htons(opcode);
    memcpy(arp->sender_mac, device->mac, sizeof(arp->sender_mac));
    memcpy(arp->sender_ip, device->ip, sizeof(arp->sender_ip));
    memcpy(arp->target_mac, target_mac, sizeof(arp->target_mac));
    memcpy(arp->target_ip, target_ip, sizeof(arp->target_ip));
    return sizeof(*arp);
}

bool ARP__request_mac(CONNECTION_t *self, IP_ADDRESS_t destination_ip)
{
    bool rc = false;
    DEVICE_t *device = self->device;
    uint8_t *buffer = (uint8_t *)device->output_packet;

    IF_FALSE_GOTO(EASY_IP__lock_mutex(&device->mutex), ErrorHandling);
    buffer += ETHER__fill(device, buffer, BROADCAST_MAC, ARP_PROTOCOL);
    buffer += ARP__fill(device, buffer, ARP_REQUEST, ARP_REQUEST_TARGET_MAC, destination_ip);
    EASY_IP__write_packet(device->output_packet, (uint32_t)buffer - (uint32_t)device->output_packet);
    IF_FALSE_GOTO(EASY_IP__unlock_mutex(&device->mutex), ErrorHandling);

    rc = true;
    goto Exit;
ErrorHandling:
    rc = false;
Exit:
    return rc;
}

void ARP__handle_request(DEVICE_t *device, ARP_t *arp)
{
    uint8_t *buffer = (uint8_t *)device->output_packet;

    if (0 != memcmp(arp->target_ip, device->ip, sizeof(arp->target_ip))) {
        goto Exit;
    }

    printf("Received arp request packet\n");
    ARP_CACHE__update(&device->arp_cache, arp->sender_ip, arp->sender_mac);

    IF_FALSE_GOTO(EASY_IP__lock_mutex(&device->mutex), Exit);
    buffer += ETHER__fill(device, buffer, arp->sender_mac, ARP_PROTOCOL);
    buffer += ARP__fill(device, buffer, ARP_REPLY, arp->sender_mac, arp->sender_ip);
    EASY_IP__write_packet(device->output_packet, (uint32_t)buffer - (uint32_t)device->output_packet);
    IF_FALSE_GOTO(EASY_IP__unlock_mutex(&device->mutex), Exit);

Exit:
    return;
}

bool ARP__handle_response(DEVICE_t *device, ARP_t *arp)
{
    bool rc = false;
    ARP_CACHE__update(&device->arp_cache, arp->sender_ip, arp->sender_mac);
    CONN__handle_arp_response(arp->sender_ip);
    rc = true;
    goto Exit;
ErrorHandling:
    rc = false;
Exit:
    return rc;
}

void ARP__handle_packet(DEVICE_t *device, uint8_t *payload)
{
    ARP_t *arp = (ARP_t *)payload;

    if (ntohs(arp->protocol_type) != IPV4_PROTOCOL) {
        goto Exit;
    }

    /* TODO: add defines */
    if ((1 != ntohs(arp->hw_type)) || (4 != arp->protocol_size)) {
        goto Exit;
    }

    switch (ntohs(arp->opcode)) {
        case ARP_REQUEST:
            ARP__handle_request(device, arp);
            break;
        case ARP_REPLY:
            ARP__handle_response(device, arp);
            break;
    }

Exit:
    return;
}
