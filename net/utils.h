#ifndef NET_UTILS_H
#define NET_UTILS_H

#include <stdint.h>
#include <net_config.h>

#ifdef EASY_IP_LITTLE_ENDIAN
#define ntohs(__value) (uint16_t)((((uint16_t)(__value)) << 8) | (((uint16_t)(__value)) >> 8))
#else
#define ntohs(__value) (__value)
#endif
#define htons ntohs

uint16_t checksum16(uint8_t *buffer, uint16_t length);

void UTILS__fill_checksums(uint8_t *buffer);

#define IF_FALSE_GOTO(__validate_value, __label) \
    do { \
        if (!(__validate_value)) { \
            goto __label; \
        } \
    } while(0)

#define ON_NULL_GOTO(__validate_value, __label) IF_FALSE_GOTO(NULL == __validate_value, __label)

#endif
