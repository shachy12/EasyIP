#include <net/arp/arp.h>
#include <net/ipv4/ip.h>

PROTOCOL_HANDLER(ARP_PROTOCOL, eip_arp_handle_packet)
PROTOCOL_HANDLER(IPV4_PROTOCOL, eip_ip_handle_packet)
