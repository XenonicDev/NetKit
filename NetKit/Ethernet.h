/*
	Copyright (c) 2017-2018 Andrew Depke
*/
#pragma once

#include "EthernetHeader.h"

ETHERNET_HEADER* CreateEthernetHeader(char* SourceMACAddress, char* DestinationMACAddress, int Protocol)
{
	ETHERNET_HEADER* Result;

	Result = (ETHERNET_HEADER*)malloc(sizeof(ETHERNET_HEADER));

	memcpy(Result->h_source, (void*)ether_aton(SourceMACAddress), 6);
	memcpy(Result->h_dest, (void*)ether_aton(DestinationMACAddress), 6);
	Result->h_proto = htons(Protocol);

	return Result;
}