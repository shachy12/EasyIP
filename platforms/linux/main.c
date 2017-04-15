#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <platforms/linux/net_driver.h>
#include <platforms/linux/threads.h>
#include <net/device.h>
#include <net_config.h>

MAC_ADDRESS_t src_mac = {0xde, 0xad, 0xbe, 0xef, 5, 6};
IP_ADDRESS_t src_ip = {192, 168, 0, 2};
IP_ADDRESS_t subnet = {255, 255, 255, 0};
IP_ADDRESS_t gateway = {192, 168, 0, 1};

DEVICE_t device = {0};
pthread_t net_threads[THREAD_COUNT] = {0};

#define NET_DRIVER_THREAD_INDEX (0)

void run_network_threads(void);
void wait_for_network_threads_to_finish(void);

int main(void)
{
    if (!NET_DRV__init(&device, src_mac, src_ip, subnet, gateway)) {
        printf("Failed to initialize net driver\n");
        goto Exit;
    }

    /* We don't validate return values in those functions because this is just to debug */
    run_network_threads();
    wait_for_network_threads_to_finish();
Exit:
    return 0;
}

void run_network_threads(void)
{
#define THREAD_HANDLER(__thread_id, __thread_handler) \
    pthread_create(&net_threads[__thread_id], NULL, __thread_handler, &device);
#include <platforms/linux/threads.hx>
#undef THREAD_HANDLER
}

void wait_for_network_threads_to_finish(void)
{
#define THREAD_HANDLER(__thread_id, __thread_handler) \
    pthread_join(net_threads[__thread_id], NULL);
#include <platforms/linux/threads.hx>
#undef THREAD_HANDLER

}
