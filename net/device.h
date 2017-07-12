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
    eip_arp_cache_table_t arp_cache;
    uint8_t output_packet[MTU];
    uint8_t input_packet[MTU];
    EASY_IP_MUTEX_TYPE mutex;
} DEVICE_t;

bool eip_dev_init(DEVICE_t *device,
                  MAC_ADDRESS_t mac,
                  IP_ADDRESS_t ip,
                  IP_ADDRESS_t subnet_mask,
                  IP_ADDRESS_t gateway);

void eip_dev_handle_packet(DEVICE_t *device, uint16_t packet_size);

/* This function creates a mutex, returning NULL means it can't create a mutex otherwise returns the mutex identifier used to lock and unlock it */
bool eip_create_mutex(EASY_IP_MUTEX_TYPE *mutex);
bool eip_lock_mutex(EASY_IP_MUTEX_TYPE *mutex);
bool eip_unlock_mutex(EASY_IP_MUTEX_TYPE *mutex);
bool eip_destroy_mutex(EASY_IP_MUTEX_TYPE *mutex);

bool eip_create_signal(EASY_IP_SIGNAL_TYPE *signal);
bool eip_wait_signal(EASY_IP_SIGNAL_TYPE *signal);
bool eip_post_signal(EASY_IP_SIGNAL_TYPE *signal);
bool eip_destroy_signal(EASY_IP_SIGNAL_TYPE *signal);

/* This function must be called from the same context as the eip_dev_periodic_timer function */
void eip_write_packet(uint8_t *packet, uint16_t length);

/* This function must be called from the same context as the eip_dev_handle_packet function */
void eip_dev_periodic_timer(DEVICE_t *device);

#endif /* NET_DEVICE_H */
