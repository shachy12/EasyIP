#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <net/device.h>
#include <net/arp/arp_cache.h>
#include <net/protocols.h>
#include <libs/RecordsAllocator/records_allocator.h>
#include <libs/Errors/errors.h>

bool eip_arp_cache_init(eip_arp_cache_table_t *table)
{
    memset(table, 0, sizeof(*table));
    printf("Initializing arp cache\n");
    RECORDS_ALLOCATOR_INIT(&table->allocator, table->entries);
    return eip_create_mutex(&table->mutex);
}

bool eip_arp_cache_update(eip_arp_cache_table_t *table, IP_ADDRESS_t ip, MAC_ADDRESS_t mac)
{
    bool rc = false;
    eip_arp_cache_entry_t *entry = eip_arp_cache_get(table, ip);
    if (NULL != entry) {
        /* Updating the mac in the existing entry */
        goto UpdateEntry;
    }
    IF_FALSE_GOTO(eip_lock_mutex(&table->mutex), ErrorHandling);
    entry = records_allocator_allocate(&table->allocator);
    if (NULL == entry) {
        IF_FALSE_GOTO(eip_unlock_mutex(&table->mutex), ErrorHandling);
        /* TODO: Take oldest entry and use it instead */
        goto Exit;
    }
UpdateEntry:
    memcpy(entry->ip, ip, sizeof(entry->ip));
    memcpy(entry->mac, mac, sizeof(entry->mac));
    IF_FALSE_GOTO(eip_unlock_mutex(&table->mutex), ErrorHandling);
    rc = true;
    goto Exit;
    /* entry->time = CONFIG_GET_TIME(); */
ErrorHandling:
    rc = false;
Exit:
    return rc;
}

eip_arp_cache_entry_t * eip_arp_cache_get(eip_arp_cache_table_t *table, IP_ADDRESS_t ip)
{
    eip_arp_cache_entry_t *entry = NULL;
    IF_FALSE_GOTO(eip_lock_mutex(&table->mutex), ErrorHandling);
    RECORDS_ALLOCATOR_FOREACH_NODE(&table->allocator, entry) {
        if (0 == memcmp(ip, entry->ip, sizeof(entry->ip))) {
            IF_FALSE_GOTO(eip_unlock_mutex(&table->mutex), ErrorHandling);
            goto Exit;
        }
    }
    IF_FALSE_GOTO(eip_unlock_mutex(&table->mutex), ErrorHandling);
ErrorHandling:
    entry = NULL;
Exit:
    return entry;
}

bool eip_arp_cache_get_mac(eip_arp_cache_table_t *table, IP_ADDRESS_t ip, MAC_ADDRESS_t out_mac)
{
    bool rc = false;
    eip_arp_cache_entry_t *entry = eip_arp_cache_get(table, ip);
    ON_NULL_GOTO(entry, ErrorHandling);

    IF_FALSE_GOTO(eip_lock_mutex(&table->mutex), ErrorHandling);
    memcpy(out_mac, entry->mac, sizeof(entry->mac));
    IF_FALSE_GOTO(eip_unlock_mutex(&table->mutex), ErrorHandling);
    rc = true;
    goto Exit;
ErrorHandling:
    rc = false;
Exit:
    return rc;
}
