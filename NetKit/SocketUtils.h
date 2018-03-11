#pragma once

#include "Platform.h"

#include <stdlib.h>
#include <stdio.h>

int CreateSocket(int Protocol)
{
	int Result = socket(AF_INET, SOCK_RAW, Protocol);
	if (Result == -1)
	{
		printf("CreateSocket Failed\n");

		return -1;
	}

	int Option = 1;

	if (setsockopt(Result, IPPROTO_IP, IP_HDRINCL, (char*)&Option, sizeof(Option)) != 0)
	{
		printf("CreateSocket Failed at Setting Custom Headers\n");

		return Result;
	}

	return Result;
}

int BindSocket(int Socket, unsigned long Address, int Port)
{
	struct sockaddr_in Data;
	Data.sin_family = AF_INET;
	Data.sin_addr.s_addr = Address;
	Data.sin_port = htons(Port);

	return bind(Socket, (struct sockaddr*)&Data, sizeof(Data));
}

int SendPacket(int Socket, unsigned char* Buffer, size_t BufferLength)
{
	int Sent = send(Socket, (char*)Buffer, BufferLength, 0);

	if (Sent != BufferLength)
	{
		printf("SendPacket Warning: Total Bytes Sent Did Not Match Size of Packet Buffer. Total Bytes Sent: %d\n", Sent);

		return 1;
	}

	return 0;
}