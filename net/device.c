#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <net/device.h>
#include <net/protocols.h>
#include <net/ether.h>
#include <net/arp/arp.h>
#include <net/arp/arp_cache.h>
#include <net/utils.h>

bool DEVICE__init(DEVICE_t *device,
                  MAC_ADDRESS_t mac,
                  IP_ADDRESS_t ip,
                  IP_ADDRESS_t subnet_mask,
                  IP_ADDRESS_t gateway)
{
    memset(device, 0, sizeof(*device));
    memcpy(device->mac, mac, sizeof(device->mac));
    memcpy(device->ip, ip, sizeof(device->ip));
    memcpy(device->subnet_mask, subnet_mask, sizeof(device->subnet_mask));
    memcpy(device->gateway, gateway, sizeof(device->gateway));
    ARP_CACHE__init(&device->arp_cache);
    return EASY_IP__create_mutex(&device->mutex);
}


void DEVICE__handle_packet(DEVICE_t *device, uint16_t packet_size)
{
    ETHER_t *ether = (ETHER_t *)device->input_packet;

    if (0 != memcmp(ether->destination, device->mac, sizeof(device->mac)) &&
        0 != memcmp(ether->destination, BROADCAST_MAC, sizeof(ether->destination))) {
        goto Exit;
    }

#define PROTOCOL_HANDLER(__protocol, __handler) \
    if (ntohs(ether->protocol) == __protocol) { \
        __handler(device, ether->payload); \
    }
#include <net/ether_protocol_handlers.hx>
#undef PROTOCOL_HANDLER

Exit:
    return;
}

void DEVICE__periodic_timer(DEVICE_t *device)
{
    /* printf("Timer called\n"); */
}
