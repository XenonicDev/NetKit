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
	void* SourceMAC = NULL;
	void* DestinationMAC = NULL;

	RtlEthernetStringToAddress(SourceMACAddress, NULL, SourceMAC);
	RtlEthernetStringToAddress(DestinationMACAddress, NULL, DestinationMAC);

	memcpy(Result->h_source, SourceMAC, 6);
	memcpy(Result->h_dest, DestinationMAC, 6);
#else
	memcpy(Result->h_source, (void*)ether_aton(SourceMACAddress), 6);
	memcpy(Result->h_dest, (void*)ether_aton(DestinationMACAddress), 6);
#endif
	Result->h_proto = htons(Protocol);

	return Result;
}