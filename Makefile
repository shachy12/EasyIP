PLATFORM=linux

CC=gcc
CFLAGS=-Werror -pedantic -O2 -I. -Iplatforms/$(PLATFORM) -pthread -lpthread
FILES=platforms/$(PLATFORM)/main.c \
	  platforms/$(PLATFORM)/net_driver.c \
	  net/device.c \
	  net/ether.c \
	  net/arp/arp.c \
	  net/arp/arp_cache.c \
	  net/ipv4/ip.c \
	  net/ipv4/icmp.c \
	  net/ipv4/udp.c \
	  net/utils.c

$(PLATFORM): $(FILES)
	gcc $(CFLAGS) -o $(PLATFORM) $(FILES)

clean:
	rm $(PLATFORM)
