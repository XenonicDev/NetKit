/*
	Copyright (c) 2017-2018 Andrew Depke
*/
#pragma once

#include "EthernetHeader.h"

ETHERNET_HEADER* CreateEthernetHeader(char* SourceMACAddress, char* DestinationMACAddress, int Protocol)
{
	ETHERNET_HEADER* Result;

	Result = (ETHERNET_HEADER*)malloc(sizeof(ETHERNET_HEADER));

#ifdef PLATFORM_WINDOWS
	unsigned char SourceMAC[6];
	unsigned char DestinationMAC[6];

	sscanf_s(SourceMACAddress, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &SourceMAC[0], &SourceMAC[1], &SourceMAC[2], &SourceMAC[3], &SourceMAC[4], &SourceMAC[5]);
	sscanf_s(DestinationMACAddress, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &DestinationMAC[0], &DestinationMAC[1], &DestinationMAC[2], &DestinationMAC[3], &DestinationMAC[4], &DestinationMAC[5]);

	memcpy(Result->h_source, SourceMAC, 6);
	memcpy(Result->h_dest, DestinationMAC, 6);
#else
	struct ether_addr* SourceMAC = ether_aton(SourceMACAddress);
	struct ether_addr* DestinationMAC = ether_aton(DestinationMACAddress);

	memcpy(Result->h_source, SourceMAC->ether_addr_octet, 6);
	memcpy(Result->h_dest, DestinationMAC->ether_addr_octet, 6);
#endif
	Result->h_proto = htons(Protocol);

	return Result;
}