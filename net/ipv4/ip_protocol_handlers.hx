#include <net/ipv4/icmp.h>
#include <net/ipv4/udp.h>

IP_PROTOCOL_HANDLER(IP__ICMP_PROTOCOL, ICMP__handle_packet)
IP_PROTOCOL_HANDLER(IP__UDP_PROTOCOL, UDP__handle_packet)
