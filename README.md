# EasyIP
A TCP/IP stack implementation written in C.

This project is still in progress.

## What is implemented?
* ARP Cache
* ARP response + request
* ICMPv4 - ping response
* UDP listen, sendto, recvfrom (Implementation of echo server in platforms/linux/udp_server.c)

## Need to do
* TCP
* IP fragmentation
* ARP Cache needs to reuse old entries if all entries are used
* ICMP Unreachable
* UDP connect function, you can still send udp to another server using listen and sendto.
* Better error handling
