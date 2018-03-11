/*
	Copyright (c) 2017-2018 Andrew Depke
*/
#pragma once

#include "Platform.h"

#ifdef PLATFORM_UNIX
typedef struct ethhdr ETHERNET_HEADER;
#else
#define ETH_ALEN 6  // Octets in a Single Ethernet Address

struct ETHERNET_HEADER
{
	unsigned char h_dest[ETH_ALEN];  // Destination Ethernet Address
	unsigned char h_source[ETH_ALEN];  // Source Ethernet Address
	unsigned short h_proto;  // Protocol
};
#endif