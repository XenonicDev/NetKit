/*
	Copyright (c) 2017-2018 Andrew Depke
*/
#pragma once

#if defined(_WIN32) || defined(_WIN64)
#define PLATFORM_WINDOWS

#error Windows Is No Longer Supported

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")
#else
#define PLATFORM_UNIX

#include <unistd.h>
#include <stddef.h>  // size_t
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <arpa/inet.h>
#include <string.h>  // bzero(), strncpy_s()

using std::size_t;
#endif