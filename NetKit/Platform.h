/*
	Copyright (c) 2017-2018 Andrew Depke
*/
#pragma once

#include <errno.h>

#if defined(_WIN32) || defined(_WIN64)
#define PLATFORM_WINDOWS

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "IPHLPAPI.lib")
#else
#define PLATFORM_UNIX

#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <string.h>  // bzero(), strncpy_s(), strtok_r()
#endif