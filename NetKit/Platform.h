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

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <arpa/inet.h>
#include <strings.h>  // bzero()
#endif