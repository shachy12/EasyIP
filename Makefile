PLATFORM=linux

CC=gcc
CFLAGS=-Werror -pedantic -O2 -I. -Iplatforms/$(PLATFORM) -pthread -lpthread
FILES=platforms/$(PLATFORM)/main.c \
	  platforms/$(PLATFORM)/net_driver.c \
	  platforms/$(PLATFORM)/udp_server.c \
	  net/device.c \
	  net/ether.c \
	  net/arp/arp.c \
	  net/arp/arp_cache.c \
	  net/ipv4/ip.c \
	  net/ipv4/icmp.c \
	  net/ipv4/udp.c \
	  net/conn.c \
	  net/utils.c \
	  libs/LinkedList/LINKED_LIST.c \
	  libs/CyclicBuffer/CYCLIC_BUFFER.c

$(PLATFORM): $(FILES)
	gcc $(CFLAGS) -o $(PLATFORM) $(FILES)

clean:
	rm $(PLATFORM)
