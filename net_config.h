#ifndef NET_CONFIG_H
#define NET_CONFIG_H

#include <pthread.h>

/* For Big Endian architectures remove this define */
#define EASY_IP_LITTLE_ENDIAN
/* The ttl of the ip packets */
#define EASY_IP_TTL (0xff)

/* Arp Cache size 100 means we can hold 100 different addresses */
#define EASY_IP_ARP_CACHE_SIZE (100)

/* The priodic timer speed in milliseconds */
#define EASY_IP_PERIODIC_TIMER_SPEED (1000)

#define EASY_IP_MUTEX_TYPE pthread_mutex_t

#endif
