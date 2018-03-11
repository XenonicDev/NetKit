#pragma once

#include "Platform.h"
#include "SysUtils.h"
#include "IPHeader.h"
#include "TCPHeader.h"

struct IP_HEADER* CreateIPHeader(int Protocol, unsigned long SourceAddress, unsigned long DestinationAddress, size_t PayloadLength)
{
	struct IP_HEADER* Result;

	Result = (IP_HEADER*)malloc(sizeof(IP_HEADER));

	Result->version = 4;
	Result->ihl = (sizeof(struct IP_HEADER)) / 4;  // Divide into Words. No Options.
	Result->tos = 0;
	Result->tot_len = htons(sizeof(struct IP_HEADER) + sizeof(struct TCP_HEADER) + PayloadLength);
	Result->id = 0;
	Result->frag_off = 0;
	Result->ttl = htons(128);
	Result->protocol = Protocol;
	Result->check = 0;  // Calculate this Later.
	Result->saddr = SourceAddress;
	Result->daddr = DestinationAddress;

	Result->check = CalculateChecksum((unsigned char*)Result, Result->ihl * 4);

	return Result;
}