#ifndef LINUX_NET_DRIVER
#define LINUX_NET_DRIVER

#include <stdbool.h>
#include <net/device.h>

bool NET_DRV__init(DEVICE_t *device,
                   MAC_ADDRESS_t src_mac,
                   IP_ADDRESS_t src_ip,
                   IP_ADDRESS_t subnet,
                   IP_ADDRESS_t gateway);
void *NET_DRV__thread(void *arg);

#endif
