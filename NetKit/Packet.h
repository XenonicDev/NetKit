/*
	Copyright (c) 2017-2018 Andrew Depke
*/
#pragma once

#include "Platform.h"
#include "IPHeader.h"
#include "TCPHeader.h"
#include "PseudoHeader.h"
#include "SysUtils.h"

struct Packet
{
	unsigned char* Raw;
	size_t Length;
};

struct Packet* CreateTCPPacket(struct IP_HEADER* IPHeader, struct TCP_HEADER* TCPHeader, unsigned char* Payload, size_t PayloadLength)
{
	struct Packet* Result;

	int TCPAndPayloadLength = ntohs(IPHeader->tot_len) - IPHeader->ihl * 4;
	int PseudoPacketLength = sizeof(struct PSEUDO_HEADER) + TCPAndPayloadLength;  // Size of the Packet With a Pseudo Header.

	unsigned char* PseudoPacket = (unsigned char*)malloc(PseudoPacketLength);

	struct PSEUDO_HEADER* PseudoHeader = (struct PSEUDO_HEADER*)PseudoPacket;

	PseudoHeader->SourceAddress = IPHeader->saddr;
	PseudoHeader->DestinationAddress = IPHeader->daddr;
	PseudoHeader->Reserved = 0;
	PseudoHeader->Protocol = IPHeader->protocol;
	PseudoHeader->Length = htons(TCPAndPayloadLength);  // Size of the Packet Minus the IP Header.

	memcpy((PseudoPacket + sizeof(struct PSEUDO_HEADER)), TCPHeader, TCPHeader->doff * 4);  // Copy TCP Header.
	memcpy((PseudoPacket + sizeof(struct PSEUDO_HEADER) + TCPHeader->doff * 4), Payload, PayloadLength);  // Copy Payload.

	TCPHeader->check = CalculateChecksum(PseudoPacket, PseudoPacketLength);

	free(PseudoHeader);

	Result = (struct Packet*)malloc(sizeof(struct Packet));

	Result->Raw = (unsigned char*)malloc(ntohs(IPHeader->tot_len));

	size_t Offset = 0;

	memcpy((Result->Raw + Offset), IPHeader, IPHeader->ihl * 4);

	Offset += IPHeader->ihl * 4;

	memcpy((Result->Raw + Offset), TCPHeader, TCPHeader->doff * 4);

	Offset += TCPHeader->doff * 4;

	memcpy((Result->Raw + Offset), Payload, PayloadLength);

	return Result;
}

void CleanupPacket(struct Packet* Target)
{
	free(Target->Raw);
}