/*
	Copyright (c) 2017-2018 Andrew Depke
*/
#pragma once

#include "Platform.h"
#include "Packet.h"

#include <stdlib.h>
#include <stdio.h>

int CreateSocket(int Protocol)
{
	int Result = socket(AF_INET, (Protocol == IPPROTO_UDP ? SOCK_DGRAM : SOCK_STREAM), Protocol);
	if (Result == -1)
	{
		printf("CreateSocket Failed\n");

		return -1;
	}

	return Result;
}

int CreateSocketRaw(int Protocol, int CustomHeaders)
{
	int Result = socket(PF_PACKET, SOCK_RAW, htons(Protocol));
	if (Result == -1)
	{
		printf("CreateSocketRaw Failed\n");

		return -1;
	}

	if (CustomHeaders)
	{
		int Option = 1;

		if (setsockopt(Result, IPPROTO_IP, IP_HDRINCL, (char*)&Option, sizeof(Option)) != 0)
		{
			printf("CreateSocketRaw Failed at Setting Custom Headers\n");

			return Result;
		}
	}

	return Result;
}

int BindSocket(int Socket, unsigned long Address, int Port)
{
	struct sockaddr_in Data;
	Data.sin_family = AF_PACKET;
	Data.sin_addr.s_addr = htonl(Address);
	Data.sin_port = htons(Port);

	if (bind(Socket, (struct sockaddr*)&Data, sizeof(Data)) != 0)
	{
		printf("BindSocket Failed\n");

		return -1;
	}

	return 0;
}

int BindSocketRaw(int Socket, char* Device, int Protocol)
{
	struct sockaddr_ll LinkLayerAddress;
	struct ifreq ifr;

	bzero(&LinkLayerAddress, sizeof(LinkLayerAddress));
	bzero(&ifr, sizeof(ifr));

	strncpy((char*)ifr.ifr_name, Device, 16);

	if (ioctl(Socket, SIOCGIFINDEX, &ifr) != 0)
	{
		printf("BindSocketRaw Failed at Network Interface Index Retrieval\n");

		return -1;
	}

	LinkLayerAddress.sll_family = AF_PACKET;
	LinkLayerAddress.sll_ifindex = ifr.ifr_ifindex;
	LinkLayerAddress.sll_protocol = htons(Protocol);

	if (bind(Socket, (struct sockaddr*)&LinkLayerAddress, sizeof(LinkLayerAddress)) != 0)
	{
		printf("BindSocketRaw Failed\n");

		return -1;
	}

	return 0;
}

int SendPacket(int Socket, unsigned long DestinationAddress, int DestinationPort, unsigned char* Payload, size_t PayloadLength)
{
	struct sockaddr_in Destination;
	Destination.sin_addr.s_addr = htonl(DestinationAddress);
	Destination.sin_port = htons(DestinationPort);

	int Sent = sendto(Socket, (char*)Payload, (int)PayloadLength, 0, (struct sockaddr*)&Destination, sizeof(Destination));

	if (Sent != (int)PayloadLength)
	{
		return -1;
	}

	return 0;
}

int SendPacketRaw(int Socket, Packet* TargetPacket)
{
	int Sent = write(Socket, TargetPacket->Raw, (int)TargetPacket->Length);

	if (Sent != (int)TargetPacket->Length)
	{
		return -1;
	}

	return 0;
}

void ShutdownSocket(int Socket)
{
	shutdown(Socket, 2);
#ifdef PLATFORM_WINDOWS
	closesocket(Socket);
#else
	close(Socket);
#endif
}