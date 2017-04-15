#ifndef NET_UTILS_H
#define NET_UTILS_H

#include <stdint.h>
#include <net_config.h>

#ifdef EASY_IP_LITTLE_ENDIAN
#define ntohs(__value) (uint16_t)((((uint16_t)(__value)) << 8) | (((uint16_t)(__value)) >> 8))
#else
/* #define ntohs(__value) (__value) */
#endif
#define htons ntohs

uint16_t checksum16(uint8_t *buffer, uint16_t length);

void UTILS__fill_checksums(uint8_t *buffer);

#endif
