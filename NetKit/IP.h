/*
	Copyright (c) 2017-2018 Andrew Depke
*/
#pragma once

#include "Platform.h"
#include "SysUtils.h"
#include "IPHeader.h"
#include "TCPHeader.h"

IP_HEADER* CreateIPHeader(int Protocol, unsigned long SourceAddress, unsigned long DestinationAddress, size_t PayloadLength)
{
	IP_HEADER* Result;

	Result = (IP_HEADER*)malloc(sizeof(IP_HEADER));

	Result->version = 4;
	Result->ihl = (sizeof(IP_HEADER)) / 4;  // Divide into Words. No Options.
	Result->tos = 0;
	Result->tot_len = htons(sizeof(IP_HEADER) + sizeof(TCP_HEADER) + PayloadLength);
	Result->id = 0;
	Result->frag_off = 0;
	Result->ttl = htons(128);
	Result->protocol = Protocol;
	Result->check = 0;  // Calculate this Later.
	Result->saddr = htonl(SourceAddress);
	Result->daddr = htonl(DestinationAddress);

	Result->check = (unsigned short)CalculateChecksum((unsigned char*)Result, Result->ihl * 4);

	return Result;
}