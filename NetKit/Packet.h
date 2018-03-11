#pragma once

#include "Platform.h"
#include "IPHeader.h"
#include "TCPHeader.h"

struct Packet
{
	unsigned char* Raw;
	size_t Length;
};

struct Packet* CreateTCPPacket(struct IP_HEADER* IPHeader, struct TCP_HEADER* TCPHeader, unsigned char* Payload, size_t PayloadLength)
{
	Packet* Result;

	Result->Raw = (unsigned char*)malloc(ntohs(IPHeader->tot_len));

	size_t Offset = 0;

	memcpy(Result->Raw, IPHeader, IPHeader->ihl * 4);

	Offset += IPHeader->ihl * 4;

	memcpy(Result->Raw + Offset, TCPHeader, TCPHeader->doff * 4);

	Offset += TCPHeader->doff * 4;

	memcpy(Result->Raw + Offset, Payload, PayloadLength);

	return Result;
}

void CleanupPacket(struct Packet* Target)
{
	free(Target->Raw);
}