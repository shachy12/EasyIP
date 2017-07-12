#include <net/ipv4/icmp.h>
#include <net/ipv4/udp.h>

IP_PROTOCOL_HANDLER(eip_ip_ICMP_PROTOCOL, eip_icmp_handle_packet)
IP_PROTOCOL_HANDLER(eip_ip_UDP_PROTOCOL, eip_udp_handle_packet)
