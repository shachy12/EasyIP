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

bool NET_DRV__init(DEVICE_t *device,
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

    return DEVICE__init(device, src_mac, src_ip, subnet, gateway);
Error:
    return false;
}

void *NET_DRV__thread(void *arg)
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
            DEVICE__handle_packet(device, received_bytes);
        }

        if (-1 == gettimeofday(&current_time, NULL)) {
            printf("Error in getting the time\n");
            goto Exit;
        }

        /* TODO: Find a better way for timers in the same context */
        if ((current_time.tv_sec * 1000) + (current_time.tv_usec / 1000) -
            ((last_time.tv_sec * 1000) + (last_time.tv_usec / 1000)) >= EASY_IP_PERIODIC_TIMER_SPEED ) {
            /* This function must be called from the same context as the DEVICE__handle_packet function */
            DEVICE__periodic_timer(device);
            memcpy(&last_time, &current_time, sizeof(current_time));
        }

    }
Exit:
    return NULL;
}

void EASY_IP__write_packet(uint8_t *packet, uint16_t length)
{
    int wrote_bytes = write(tapdev_fd, packet, length);
    printf("Wrote packet, %d bytes\n", wrote_bytes);
    assert(wrote_bytes == length);
}

bool EASY_IP__create_mutex(EASY_IP_MUTEX_TYPE *mutex)
{
    printf("Created mutex\n");
    return 0 == pthread_mutex_init(mutex, NULL);
}

bool EASY_IP__lock_mutex(EASY_IP_MUTEX_TYPE *mutex)
{
    printf("Locked mutex\n");
    return 0 ==  pthread_mutex_lock(mutex);
}

bool EASY_IP__unlock_mutex(EASY_IP_MUTEX_TYPE *mutex)
{
    printf("Unlocked mutex\n");
    return 0 ==  pthread_mutex_unlock(mutex);
}

bool EASY_IP__destroy_mutex(EASY_IP_MUTEX_TYPE *mutex)
{
    printf("destroyed mutex\n");
    return 0 == pthread_mutex_destroy(mutex);
}

bool EASY_IP__create_signal(EASY_IP_SIGNAL_TYPE *signal)
{
    printf("Created signal\n");
    return 0 == sem_init(signal, 0, 0);
}

bool EASY_IP__wait_signal(EASY_IP_SIGNAL_TYPE *signal)
{
    printf("Waiting for signal\n");
    return 0 == sem_wait(signal);
}

bool EASY_IP__post_signal(EASY_IP_SIGNAL_TYPE *signal)
{
    printf("Sent signal\n");
    return 0 == sem_post(signal);
}

bool EASY_IP__destroy_signal(EASY_IP_SIGNAL_TYPE *signal)
{
    printf("destroyed signal\n");
    return 0 == sem_destroy(signal);
}
