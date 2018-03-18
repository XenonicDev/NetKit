/*
	Copyright (c) 2017-2018 Andrew Depke
*/
#pragma once

#include "Platform.h"
#include "EthernetHeader.h"
#include "IPHeader.h"
#include "TCPHeader.h"
#include "PseudoHeader.h"
#include "SysUtils.h"

typedef struct
{
	unsigned char* Raw;
	size_t Length;
} Packet;

Packet* CreateTCPPacket(ETHERNET_HEADER* EthernetHeader, IP_HEADER* IPHeader, TCP_HEADER* TCPHeader, unsigned char* Payload, size_t PayloadLength)
{
	Packet* Result;

	int TCPAndPayloadLength = ntohs(IPHeader->tot_len) - IPHeader->ihl * 4;
	int PseudoPacketLength = sizeof(PSEUDO_HEADER) + TCPAndPayloadLength;  // Size of the Packet With a Pseudo Header.

	unsigned char* PseudoPacket = (unsigned char*)malloc(PseudoPacketLength);

	PSEUDO_HEADER* PseudoHeader = (PSEUDO_HEADER*)PseudoPacket;

	PseudoHeader->SourceAddress = IPHeader->saddr;
	PseudoHeader->DestinationAddress = IPHeader->daddr;
	PseudoHeader->Reserved = 0;
	PseudoHeader->Protocol = IPHeader->protocol;
	PseudoHeader->Length = htons(TCPAndPayloadLength);  // Size of the Packet Minus the IP Header.

	memcpy((PseudoPacket + sizeof(PSEUDO_HEADER)), TCPHeader, TCPHeader->doff * 4);  // Copy TCP Header.
	memcpy((PseudoPacket + sizeof(PSEUDO_HEADER) + TCPHeader->doff * 4), Payload, PayloadLength);  // Copy Payload.

	TCPHeader->check = (unsigned short)CalculateChecksum(PseudoPacket, PseudoPacketLength);

	free(PseudoHeader);

	Result = (Packet*)malloc(sizeof(Packet));

	Result->Length = sizeof(ETHERNET_HEADER) + ntohs(IPHeader->tot_len);
	Result->Raw = (unsigned char*)malloc(Result->Length);

	size_t Offset = 0;

	memcpy((Result->Raw + Offset), EthernetHeader, sizeof(ETHERNET_HEADER));

	Offset += sizeof(ETHERNET_HEADER);

	memcpy((Result->Raw + Offset), IPHeader, IPHeader->ihl * 4);

	Offset += IPHeader->ihl * 4;

	memcpy((Result->Raw + Offset), TCPHeader, TCPHeader->doff * 4);

	Offset += TCPHeader->doff * 4;

	memcpy((Result->Raw + Offset), Payload, PayloadLength);

	return Result;
}

void CleanupPacket(Packet* Target)
{
	free(Target->Raw);
}