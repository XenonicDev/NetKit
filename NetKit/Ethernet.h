/*
	Copyright (c) 2017-2018 Andrew Depke
*/
#pragma once

#include "EthernetHeader.h"

struct ETHERNET_HEADER* CreateEthernetHeader(char* SourceMACAddress, char* DestinationMACAddress, int Protocol)
{
	struct ETHERNET_HEADER* Result;

	Result = (struct ETHERNET_HEADER*)malloc(sizeof(struct ETHERNET_HEADER));

	memcpy(Result->h_source, (void*)ether_aton(SourceMACAddress), 6);
	memcpy(Result->h_dest, (void*)ether_aton(DestinationMACAddress), 6);
	Result->h_proto = htons(Protocol);

	return Result;
}