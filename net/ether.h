#ifndef NET_ETHER_H
#define NET_ETHER_H

#include <stdint.h>
#include <net/device.h>

uint16_t ETHER__fill(DEVICE_t *device, uint8_t *buffer, MAC_ADDRESS_t destination_mac, uint16_t protocol);

#endif
