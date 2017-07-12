#ifndef NET_ARP_CACHE_H
#define NET_ARP_CACHE_H

#include <stdbool.h>
#include <stdint.h>
#include <net/protocols.h>
#include <net_config.h>
#include <libs/RecordsAllocator/records_allocator.h>

typedef struct eip_arp_cache_entry_s {
    RECORDS_ALLOCATOR_NODE;
    MAC_ADDRESS_t mac;
    IP_ADDRESS_t ip;
    uint32_t time;
} eip_arp_cache_entry_t;

typedef struct {
    eip_arp_cache_entry_t entries[EASY_IP_ARP_CACHE_SIZE];
    RECORDS_ALLOCATOR_t allocator;
    EASY_IP_MUTEX_TYPE mutex;
} eip_arp_cache_table_t;

bool eip_arp_cache_init(eip_arp_cache_table_t *table);

bool eip_arp_cache_update(eip_arp_cache_table_t *table, IP_ADDRESS_t ip, MAC_ADDRESS_t mac);

eip_arp_cache_entry_t * eip_arp_cache_get(eip_arp_cache_table_t *table, IP_ADDRESS_t ip);

eip_arp_cache_entry_t *eip_arp_cache_allocate(eip_arp_cache_table_t *table);

bool eip_arp_cache_get_mac(eip_arp_cache_table_t *table, IP_ADDRESS_t ip, MAC_ADDRESS_t out_mac);

#endif
