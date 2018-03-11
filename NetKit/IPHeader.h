/*
	Copyright (c) 2017-2018 Andrew Depke
*/
#pragma once

#include "Platform.h"

#ifdef PLATFORM_UNIX
typedef struct iphdr IP_HEADER;
#else
struct IP_HEADER
{
	unsigned char ihl : 4;  // Header Length
	unsigned char version : 4;  // Header Version
	unsigned char tos;  // Service Type
	unsigned short tot_len;  // Total Packet Length
	unsigned short id;  // Identification
	unsigned short frag_off;  // Fragment Offset
	unsigned char ttl;  // Time to Live
	unsigned char protocol;  // Protocol
	unsigned short check;  // Checksum
	unsigned int saddr;  // Source Address
	unsigned int daddr;  // Destination Address

	// Options...
};
#endif