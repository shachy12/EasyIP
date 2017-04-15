#ifndef NET_DEVICE_H
#define NET_DEVICE_H

#include <stdint.h>
#include <net/protocols.h>
#include <net/arp/arp_cache.h>
#include <stdbool.h>
#include <net_config.h>

typedef struct {
    MAC_ADDRESS_t mac;
    IP_ADDRESS_t ip;
    IP_ADDRESS_t subnet_mask;
    IP_ADDRESS_t gateway;
    ARP_CACHE__table_t arp_cache;
    uint8_t output_packet[MTU];
    uint8_t input_packet[MTU];
    EASY_IP_MUTEX_TYPE mutex;
} DEVICE_t;

bool DEVICE__init(DEVICE_t *device,
                  MAC_ADDRESS_t mac,
                  IP_ADDRESS_t ip,
                  IP_ADDRESS_t subnet_mask,
                  IP_ADDRESS_t gateway);

void DEVICE__handle_packet(DEVICE_t *device, uint16_t packet_size);

/* This function creates a mutex, returning NULL means it can't create a mutex otherwise returns the mutex identifier used to lock and unlock it */
bool EASY_IP__create_mutex(EASY_IP_MUTEX_TYPE *mutex);
bool EASY_IP__lock_mutex(EASY_IP_MUTEX_TYPE *mutex);
bool EASY_IP__unlock_mutex(EASY_IP_MUTEX_TYPE *mutex);
bool EASY_IP__destroy_mutex(EASY_IP_MUTEX_TYPE *mutex);

bool EASY_IP__create_signal(EASY_IP_SIGNAL_TYPE *signal);
bool EASY_IP__wait_signal(EASY_IP_SIGNAL_TYPE *signal);
bool EASY_IP__post_signal(EASY_IP_SIGNAL_TYPE *signal);
bool EASY_IP__destroy_signal(EASY_IP_SIGNAL_TYPE *signal);

/* This function must be called from the same context as the DEVICE__periodic_timer function */
void EASY_IP__write_packet(uint8_t *packet, uint16_t length);

/* This function must be called from the same context as the DEVICE__handle_packet function */
void DEVICE__periodic_timer(DEVICE_t *device);

#endif /* NET_DEVICE_H */
