#pragma once

#include "Platform.h"
#include "TCPHeader.h"

struct TCP_HEADER* CreateTCPHeader(int SourcePort, int DestinationPort, int Sequence, int Acknowledgement, int Flags)
{
	struct TCP_HEADER* Result;

	Result = (struct TCP_HEADER*)malloc(sizeof(struct TCP_HEADER));

	Result->source = htons(SourcePort);
	Result->dest = htons(DestinationPort);
	Result->seq = htonl(Sequence);
	Result->ack_seq = htonl(Acknowledgement);
	Result->res1 = 0;
	Result->doff = (sizeof(struct TCP_HEADER)) / 4;  // Divide into Words.
	Result->syn = 1;
	Result->window = htons(8192);
	Result->check = 0;  // Calculate this Later.
	Result->urg_ptr = 0;

	return Result;
}