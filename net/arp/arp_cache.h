#ifndef NET_ARP_CACHE_H
#define NET_ARP_CACHE_H

#include <stdint.h>
#include <net/protocols.h>
#include <net_config.h>

typedef struct ARP_CACHE__entry_s {
    struct ARP_CACHE__entry_s *next;
    struct ARP_CACHE__entry_s *prev;
    MAC_ADDRESS_t mac;
    IP_ADDRESS_t ip;
    uint32_t time;
} ARP_CACHE__entry_t;

typedef struct {
    ARP_CACHE__entry_t *allocated_head;
    ARP_CACHE__entry_t *oldest_allocated_head;
    ARP_CACHE__entry_t *free_head;
    ARP_CACHE__entry_t entries[CONFIG_ARP_CACHE_SIZE];
} ARP_CACHE__table_t;

void ARP_CACHE__init(ARP_CACHE__table_t *table);

void ARP_CACHE__update(ARP_CACHE__table_t *table, IP_ADDRESS_t ip, MAC_ADDRESS_t mac);

ARP_CACHE__entry_t * ARP_CACHE__get(ARP_CACHE__table_t *table, IP_ADDRESS_t ip);

ARP_CACHE__entry_t *ARP_CACHE__allocate(ARP_CACHE__table_t *table);

#endif
