/*
	Copyright (c) 2017-2018 Andrew Depke
*/
#pragma once

#include "Platform.h"
#include "TCPHeader.h"

TCP_HEADER* CreateTCPHeader(int SourcePort, int DestinationPort, int Sequence, int Acknowledgement)
{
	TCP_HEADER* Result;

	Result = (TCP_HEADER*)malloc(sizeof(TCP_HEADER));

	Result->source = htons(SourcePort);
	Result->dest = htons(DestinationPort);
	Result->seq = htonl(Sequence);
	Result->ack_seq = htonl(Acknowledgement);
	Result->res1 = 0;
	Result->doff = (sizeof(TCP_HEADER)) / 4;  // Divide into Words.
	Result->syn = 1;
	Result->window = htons(8192);
	Result->check = 0;  // Calculate this Later.
	Result->urg_ptr = 0;

	return Result;
}