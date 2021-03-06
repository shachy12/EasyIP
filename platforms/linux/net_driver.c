#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include <fcntl.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <sys/time.h>

#include <net_config.h>
#include <net/device.h>

#define TAP_DEVICE ("/dev/net/tun")

static int tapdev_fd = -1;
void handle_packets(DEVICE_t *device);

bool ned_drv_init(DEVICE_t *device,
                   MAC_ADDRESS_t src_mac,
                   IP_ADDRESS_t src_ip,
                   IP_ADDRESS_t subnet,
                   IP_ADDRESS_t gateway)
{
    tapdev_fd = open(TAP_DEVICE, O_RDWR);
    if (-1 == tapdev_fd) {
        printf("Tap device error\n");
        goto Error;
    }

    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    if (ioctl(tapdev_fd, TUNSETIFF, (void *) &ifr) < 0) {
        printf("ioctl failed\n");
        goto Error;
    }

    if (0 != system("ifconfig tap0 inet 192.168.0.1")) {
        printf("Failed to run ifconfig\n");
        goto Error;
    }

    return eip_dev_init(device, src_mac, src_ip, subnet, gateway);
Error:
    return false;
}

void *ned_drv_thread(void *arg)
{
    DEVICE_t *device = (DEVICE_t *)arg;
    fd_set fdset;
    struct timeval tv, current_time, last_time;
    int ret;
    uint32_t received_bytes = 0;

    if (-1 == gettimeofday(&last_time, NULL)) {
        printf("Error in getting the time\n");
        goto Exit;
    }

    while (true) {
        tv.tv_sec = 0;
        tv.tv_usec = 1000;

        FD_ZERO(&fdset);
        FD_SET(tapdev_fd, &fdset);

        ret = select(tapdev_fd + 1, &fdset, NULL, NULL, &tv);
        if (0 < ret) {
            received_bytes = read(tapdev_fd, device->input_packet, sizeof(device->input_packet));
            eip_dev_handle_packet(device, received_bytes);
        }

        if (-1 == gettimeofday(&current_time, NULL)) {
            printf("Error in getting the time\n");
            goto Exit;
        }

        /* TODO: Find a better way for timers in the same context */
        if ((current_time.tv_sec * 1000) + (current_time.tv_usec / 1000) -
            ((last_time.tv_sec * 1000) + (last_time.tv_usec / 1000)) >= EASY_IP_PERIODIC_TIMER_SPEED ) {
            /* This function must be called from the same context as the eip_dev_handle_packet function */
            eip_dev_periodic_timer(device);
            memcpy(&last_time, &current_time, sizeof(current_time));
        }

    }
Exit:
    return NULL;
}

#define PRINT_PACKETS
void eip_write_packet(uint8_t *packet, uint16_t length)
{
    int wrote_bytes = write(tapdev_fd, packet, length);
    printf("Wrote packet, %d bytes\n", wrote_bytes);
#ifdef PRINT_PACKETS
    for (uint16_t i = 0; i < wrote_bytes; i++) {
        printf("%02X ", packet[i]);
    }
    printf("\n");
#endif
    assert(wrote_bytes == length);
}

bool eip_create_mutex(EASY_IP_MUTEX_TYPE *mutex)
{
    return 0 == pthread_mutex_init(mutex, NULL);
}

bool eip_lock_mutex(EASY_IP_MUTEX_TYPE *mutex)
{
    return 0 ==  pthread_mutex_lock(mutex);
}

bool eip_unlock_mutex(EASY_IP_MUTEX_TYPE *mutex)
{
    return 0 ==  pthread_mutex_unlock(mutex);
}

bool eip_destroy_mutex(EASY_IP_MUTEX_TYPE *mutex)
{
    return 0 == pthread_mutex_destroy(mutex);
}

bool eip_create_signal(EASY_IP_SIGNAL_TYPE *signal)
{
    return 0 == sem_init(signal, 0, 0);
}

bool eip_wait_signal(EASY_IP_SIGNAL_TYPE *signal)
{
    return 0 == sem_wait(signal);
}

bool eip_post_signal(EASY_IP_SIGNAL_TYPE *signal)
{
    return 0 == sem_post(signal);
}

bool eip_destroy_signal(EASY_IP_SIGNAL_TYPE *signal)
{
    return 0 == sem_destroy(signal);
}
