#ifndef NET_ARP_CACHE_H
#define NET_ARP_CACHE_H

#include <stdbool.h>
#include <stdint.h>
#include <net/protocols.h>
#include <net_config.h>
#include <libs/RecordsAllocator/RECORDS_ALLOCATOR.h>

typedef struct ARP_CACHE__entry_s {
    RECORDS_ALLOCATOR_NODE;
    MAC_ADDRESS_t mac;
    IP_ADDRESS_t ip;
    uint32_t time;
} ARP_CACHE__entry_t;

typedef struct {
    ARP_CACHE__entry_t entries[EASY_IP_ARP_CACHE_SIZE];
    RECORDS_ALLOCATOR_t allocator;
    EASY_IP_MUTEX_TYPE mutex;
} ARP_CACHE__table_t;

bool ARP_CACHE__init(ARP_CACHE__table_t *table);

bool ARP_CACHE__update(ARP_CACHE__table_t *table, IP_ADDRESS_t ip, MAC_ADDRESS_t mac);

ARP_CACHE__entry_t * ARP_CACHE__get(ARP_CACHE__table_t *table, IP_ADDRESS_t ip);

ARP_CACHE__entry_t *ARP_CACHE__allocate(ARP_CACHE__table_t *table);

bool ARP_CACHE__get_mac(ARP_CACHE__table_t *table, IP_ADDRESS_t ip, MAC_ADDRESS_t out_mac);

#endif
