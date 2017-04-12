#ifndef NET_PROTOCOLS_H
#define NET_PROTOCOLS_H

#include <stdint.h>

typedef uint8_t IP_ADDRESS_t[4];
typedef uint8_t MAC_ADDRESS_t[6];

#define BROADCAST_MAC ("\xff\xff\xff\xff\xff\xff")

#define MTU (1500)
#define MIN_PACKET_SIZE (60)

typedef enum {
    ARP_PROTOCOL = 0x806,
    IPV4_PROTOCOL = 0x800
} PROTOCOLS_t;

typedef struct {
    MAC_ADDRESS_t destination;
    MAC_ADDRESS_t source;
    uint16_t protocol;
    uint8_t payload[];
} ETHER_t;

#endif /* NET_PROTOCOLS_H */
