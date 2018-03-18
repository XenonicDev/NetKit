/*
	Copyright (c) 2017-2018 Andrew Depke
*/
#pragma once

#include "Platform.h"
#include "Packet.h"

#include "SysUtils.h"

#include <stdlib.h>
#include <stdio.h>

#ifdef PLATFORM_WINDOWS
#include "Windows/WindowsUtils.h"

pcap_t* Pcap;
#endif

typedef struct
{
	unsigned long int SourceAddress;
	unsigned long int DestinationAddress;
} ConnectionData;

int CreateSocket(int Protocol)
{
	int Result = socket(AF_INET, (Protocol == IPPROTO_UDP ? SOCK_DGRAM : SOCK_STREAM), Protocol);
	if (Result == -1)
	{
		char* Error = ErrorString(errno);

		printf("CreateSocket Failed. Error: %s\n", Error);

#ifdef PLATFORM_WINDOWS
		free(Error);
#endif

		return -1;
	}

	return Result;
}

int CreateSocketRaw(int Protocol, int CustomHeaders)
#ifdef PLATFORM_WINDOWS
{
	return 0;
}
#else
{
	int Result = socket(AF_PACKET, SOCK_RAW, htons(Protocol));
	if (Result == -1)
	{
		printf("CreateSocketRaw Failed. Error: %s\n", strerror(errno));

		return -1;
	}

	if (CustomHeaders)
	{
		int Option = 1;

		if (setsockopt(Result, IPPROTO_IP, IP_HDRINCL, (char*)&Option, sizeof(Option)) != 0)
		{
			printf("CreateSocketRaw Failed at Setting Custom Headers. Error: %s\n", strerror(errno));

			return Result;
		}
	}

	return Result;
}
#endif

int BindSocket(int Socket, unsigned long Address, int Port)
{
	struct sockaddr_in Data;
	Data.sin_family = AF_INET;
	Data.sin_addr.s_addr = htonl(Address);
	Data.sin_port = htons(Port);

	if (bind(Socket, (struct sockaddr*)&Data, sizeof(Data)) != 0)
	{
		char* Error = ErrorString(errno);

		printf("BindSocket Failed. Error: %s\n", Error);

#ifdef PLATFORM_WINDOWS
		free(Error);
#endif

		return -1;
	}

	return 0;
}

int BindSocketRaw(int Socket, char* Device, int Protocol)
#ifdef PLATFORM_WINDOWS
{
	char ErrorBuffer[PCAP_ERRBUF_SIZE + 1];

	Pcap = pcap_open(Device, 100, PCAP_OPENFLAG_PROMISCUOUS, 1000, NULL, ErrorBuffer);
	if (!Pcap)
	{
		printf("BindSocketRaw Failed. Error: %s\n", ErrorBuffer);

		return -1;
	}

	return 0;
}
#else
{
	struct sockaddr_ll LinkLayerAddress;
	struct ifreq IFRequest;

	bzero(&LinkLayerAddress, sizeof(LinkLayerAddress));
	bzero(&IFRequest, sizeof(IFRequest));

	strncpy(IFRequest.ifr_name, Device, IFNAMSIZ);

	printf("\nDEVICENAME: %s\n\n", IFRequest.ifr_name);

	if (ioctl(Socket, SIOCGIFINDEX, (void*)&IFRequest) != 0)
	{
		printf("BindSocketRaw Failed at Network Interface Index Retrieval. Error: %s\n", strerror(errno));

		return -1;
	}

	LinkLayerAddress.sll_family = AF_PACKET;
	LinkLayerAddress.sll_ifindex = IFRequest.ifr_ifindex;
	LinkLayerAddress.sll_protocol = htons(Protocol);

	if (bind(Socket, (struct sockaddr*)&LinkLayerAddress, sizeof(LinkLayerAddress)) != 0)
	{
		printf("BindSocketRaw Failed. Error: %s\n", strerror(errno));

		return -1;
	}

	return 0;
}
#endif

ConnectionData GetSocketConnectionInfo(int Socket)
{
	ConnectionData Result;
	Result.SourceAddress = 0;
	Result.DestinationAddress = 0;

	struct sockaddr_in SourceAddr;
	int SourceAddrLength = sizeof(SourceAddr);

	struct sockaddr_in DestAddr;
	int DestAddrLength = sizeof(DestAddr);

	if (getsockname(Socket, (struct sockaddr*)&SourceAddr, &SourceAddrLength) != 0)
	{
		char* Error = ErrorString(errno);

		printf("GetSocketConnectionInfo Failed. Error: %s\n", Error);

#ifdef PLATFORM_WINDOWS
		free(Error);
#endif

		return Result;
	}

	Result.SourceAddress = SourceAddr.sin_addr.s_addr;

	if (getpeername(Socket, (struct sockaddr*)&DestAddr, &DestAddrLength) != 0)
	{
		char* Error = ErrorString(errno);

		printf("GetSocketConnectionInfo Failed. Error: %s\n", Error);

#ifdef PLATFORM_WINDOWS
		free(Error);
#endif

		return Result;
	}

	Result.DestinationAddress = DestAddr.sin_addr.s_addr;

	return Result;
}

int SendPacket(int Socket, unsigned long DestinationAddress, int DestinationPort, unsigned char* Payload, size_t PayloadLength)
{
	struct sockaddr_in Destination;
	Destination.sin_addr.s_addr = htonl(DestinationAddress);
	Destination.sin_port = htons(DestinationPort);

	int Sent = sendto(Socket, (char*)Payload, (int)PayloadLength, 0, (struct sockaddr*)&Destination, sizeof(Destination));

	if (Sent != (int)PayloadLength)
	{
		char* Error = ErrorString(errno);

		printf("SendPacket Failed. Error: %s\n", Error);

#ifdef PLATFORM_WINDOWS
		free(Error);
#endif

		return -1;
	}

	return 0;
}

int SendPacketRaw(int Socket, Packet* TargetPacket)
#ifdef PLATFORM_WINDOWS
{
	if (pcap_sendpacket(Pcap, TargetPacket->Raw, TargetPacket->Length) != 0)
	{
		printf("SendPacketRaw Failed. Error: %s\n", pcap_geterr(Pcap));

		return -1;
	}

	return 0;
}
#else
{
	int Sent = write(Socket, TargetPacket->Raw, (int)TargetPacket->Length);

	if (Sent != (int)TargetPacket->Length)
	{
		return -1;
	}

	return 0;
}
#endif

void ShutdownSocket(int Socket)
{
	shutdown(Socket, 2);
#ifdef PLATFORM_WINDOWS
	closesocket(Socket);
#else
	close(Socket);
#endif
}