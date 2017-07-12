#include <stdint.h>
#include <string.h>
#include <net/device.h>
#include <net/protocols.h>
#include <net/utils.h>

uint16_t eip_ether_fill(DEVICE_t *device, uint8_t *buffer, MAC_ADDRESS_t destination_mac, uint16_t protocol)
{
    ETHER_t *ether = (ETHER_t *)buffer;
    memcpy(ether->destination, destination_mac, sizeof(ether->destination));
    memcpy(ether->source, device->mac, sizeof(ether->source));
    ether->protocol = htons(protocol);
    return sizeof(*ether);
}


