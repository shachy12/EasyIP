#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <net/arp/arp_cache.h>
#include <net/protocols.h>

void ARP_CACHE__init(ARP_CACHE__table_t *table)
{
    table->allocated_head = NULL;
    memset(table->entries, 0, sizeof(table->entries));
    for (uint16_t i = 0; i < EASY_IP_ARP_CACHE_SIZE - 1; i++) {
        table->entries[i].next = &table->entries[i + 1];
        table->entries[i + 1].prev = &table->entries[i];
        table->entries[i + 1].next = NULL;
    }
    table->free_head = table->entries;
}

void ARP_CACHE__update(ARP_CACHE__table_t *table, IP_ADDRESS_t ip, MAC_ADDRESS_t mac)
{
    ARP_CACHE__entry_t *entry = ARP_CACHE__get(table, ip);
    if (NULL != entry) {
        /* Updating the mac in the existing entry */
        goto UpdateEntry;
    }
    entry = ARP_CACHE__allocate(table);
    if (NULL == entry) {
        /* TODO: handle error */
        goto Exit;
    }
    printf("Updated arp cache\n");
UpdateEntry:
    memcpy(entry->mac, mac, sizeof(entry->mac));
    /* entry->time = CONFIG_GET_TIME(); */
Exit:
    return;
}

ARP_CACHE__entry_t * ARP_CACHE__get(ARP_CACHE__table_t *table, IP_ADDRESS_t ip)
{
    ARP_CACHE__entry_t *entry = table->allocated_head;
    while (entry != NULL) {
        if (0 == memcmp(ip, entry->ip, sizeof(entry->ip))) {
            goto Exit;
        }
        entry = entry->next;
    }
Exit:
    return entry;
}

ARP_CACHE__entry_t *ARP_CACHE__allocate(ARP_CACHE__table_t *table)
{
    ARP_CACHE__entry_t *entry = table->free_head;
    if (NULL != entry) {
        table->free_head = entry->next;
        goto AllocateEntry;
    }
    /* No free entry, using the oldest one */
    if (NULL == table->oldest_allocated_head) {
        /* Shouldn't happen.. */
        /* TODO: Add log here */
        goto Exit;
    }

    entry = table->oldest_allocated_head;
    if (NULL == entry) {
        /* Shouldn't happen.. */
        /* TODO: Add log here */
        goto Exit;
    }
    entry->prev->next = NULL;
    table->oldest_allocated_head = entry->prev;

AllocateEntry:
    entry->next = table->allocated_head;
    entry->prev = NULL;
    table->allocated_head = entry;
    if (NULL != entry->next) {
        entry->next->prev = entry;
    }
Exit:
    return entry;
}
