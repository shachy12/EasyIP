#include <net/arp/arp.h>
#include <net/ipv4/ip.h>

PROTOCOL_HANDLER(ARP_PROTOCOL, ARP__handle_packet)
PROTOCOL_HANDLER(IPV4_PROTOCOL, IP__handle_packet)
