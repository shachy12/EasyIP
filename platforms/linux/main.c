#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include <fcntl.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_tun.h>

#include <net/device.h>

#define TAP_DEVICE ("/dev/net/tun")

int tapdev_fd = -1;
void handle_packets(DEVICE_t *device);

MAC_ADDRESS_t src_mac = {0xde, 0xad, 0xbe, 0xef, 5, 6};
IP_ADDRESS_t src_ip = {192, 168, 0, 2};
IP_ADDRESS_t subnet = {255, 255, 255, 0};
IP_ADDRESS_t gateway = {192, 168, 0, 1};

int main(void)
{
    tapdev_fd = open(TAP_DEVICE, O_RDWR);
    if (-1 == tapdev_fd) {
        printf("Tap device error\n");
        goto Exit;
    }

    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    if (ioctl(tapdev_fd, TUNSETIFF, (void *) &ifr) < 0) {
        printf("ioctl failed\n");
        goto Exit;
    }

    if (0 != system("ifconfig tap0 inet 192.168.0.1")) {
        printf("Failed to run ifconfig\n");
        goto Exit;
    }

    DEVICE_t device;
    DEVICE__init(&device, src_mac, src_ip, subnet, gateway);
    handle_packets(&device);
Exit:
    return 0;
}

void handle_packets(DEVICE_t *device)
{
    fd_set fdset;
    struct timeval tv, now;
    int ret;
    uint32_t received_bytes = 0;

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
    }
Exit:
    return;
}

void write_packet(uint8_t *packet, uint16_t length)
{
    int wrote_bytes = write(tapdev_fd, packet, length);
    printf("Wrote packet, %d bytes\n", wrote_bytes);
    assert(wrote_bytes == length);
}
