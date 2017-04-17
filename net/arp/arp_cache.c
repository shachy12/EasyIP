#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <net/device.h>
#include <net/arp/arp_cache.h>
#include <net/protocols.h>
#include <libs/RecordsAllocator/RECORDS_ALLOCATOR.h>
#include <libs/Errors/ERRORS.h>

bool ARP_CACHE__init(ARP_CACHE__table_t *table)
{
    memset(table, 0, sizeof(*table));
    printf("Initializing arp cache\n");
    RECORDS_ALLOCATOR_INIT(&table->allocator, table->entries);
    return EASY_IP__create_mutex(&table->mutex);
}

bool ARP_CACHE__update(ARP_CACHE__table_t *table, IP_ADDRESS_t ip, MAC_ADDRESS_t mac)
{
    bool rc = false;
    ARP_CACHE__entry_t *entry = ARP_CACHE__get(table, ip);
    if (NULL != entry) {
        /* Updating the mac in the existing entry */
        goto UpdateEntry;
    }
    IF_FALSE_GOTO(EASY_IP__lock_mutex(&table->mutex), ErrorHandling);
    entry = RECORDS_ALLOCATOR__allocate(&table->allocator);
    if (NULL == entry) {
        IF_FALSE_GOTO(EASY_IP__unlock_mutex(&table->mutex), ErrorHandling);
        /* TODO: Take oldest entry and use it instead */
        goto Exit;
    }
UpdateEntry:
    memcpy(entry->ip, ip, sizeof(entry->ip));
    memcpy(entry->mac, mac, sizeof(entry->mac));
    IF_FALSE_GOTO(EASY_IP__unlock_mutex(&table->mutex), ErrorHandling);
    rc = true;
    goto Exit;
    /* entry->time = CONFIG_GET_TIME(); */
ErrorHandling:
    rc = false;
Exit:
    return rc;
}

ARP_CACHE__entry_t * ARP_CACHE__get(ARP_CACHE__table_t *table, IP_ADDRESS_t ip)
{
    ARP_CACHE__entry_t *entry = NULL;
    IF_FALSE_GOTO(EASY_IP__lock_mutex(&table->mutex), ErrorHandling);
    RECORDS_ALLOCATOR_FOREACH_NODE(&table->allocator, entry) {
        if (0 == memcmp(ip, entry->ip, sizeof(entry->ip))) {
            IF_FALSE_GOTO(EASY_IP__unlock_mutex(&table->mutex), ErrorHandling);
            goto Exit;
        }
    }
    IF_FALSE_GOTO(EASY_IP__unlock_mutex(&table->mutex), ErrorHandling);
ErrorHandling:
    entry = NULL;
Exit:
    return entry;
}

bool ARP_CACHE__get_mac(ARP_CACHE__table_t *table, IP_ADDRESS_t ip, MAC_ADDRESS_t out_mac)
{
    bool rc = false;
    ARP_CACHE__entry_t *entry = ARP_CACHE__get(table, ip);
    ON_NULL_GOTO(entry, ErrorHandling);

    IF_FALSE_GOTO(EASY_IP__lock_mutex(&table->mutex), ErrorHandling);
    memcpy(out_mac, entry->mac, sizeof(entry->mac));
    IF_FALSE_GOTO(EASY_IP__unlock_mutex(&table->mutex), ErrorHandling);
    rc = true;
    goto Exit;
ErrorHandling:
    rc = false;
Exit:
    return rc;
}
