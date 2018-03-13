/*
	Copyright (c) 2017-2018 Andrew Depke
*/
#pragma once

#include "Platform.h"

#include <stdlib.h>
#include <stdio.h>

char* GetInput()
{
	char* line = (char*)malloc(100), *linep = line;
	size_t lenmax = 100, len = lenmax;
	int c;

	if (line == NULL)
		return NULL;

	for (;;)
	{
		c = fgetc(stdin);
		if (c == EOF)
			break;

		if (--len == 0) {
			len = lenmax;
			char * linen = (char*)realloc(linep, lenmax *= 2);

			if (linen == NULL) {
				free(linep);
				return NULL;
			}
			line = linen + (line - linep);
			linep = linen;
		}

		if ((*line++ = c) == '\n')
			break;
	}

	*line = '\0';
	return linep;
}

void ClearScreen()
{
#ifdef PLATFORM_WINDOWS
	system("cls");
#else
	system("clear");
#endif
}

void Pause()
{
#ifdef PLATFORM_WINDOWS
	system("pause");
#else
	getchar();
#endif
}

struct ConnectionData
{
	unsigned long int SourceAddress;
	unsigned long int DestinationAddress;
};

char* AddressToString(unsigned long int Address)
{
	char* Result = (char*)malloc(INET_ADDRSTRLEN * sizeof(char));

	struct sockaddr_in AddressAsSockaddr;
	AddressAsSockaddr.sin_addr.s_addr = Address;

	inet_ntop(AF_PACKET, &AddressAsSockaddr, Result, INET_ADDRSTRLEN);

	return Result;
}

char* GetHostName()
{
	char* Result = (char*)malloc(128 * sizeof(char));

	if (gethostname(Result, 128) != 0)
	{
		printf("GetHostName Failed. Error: %s\n", strerror(errno));
	}

	return Result;
}

struct ConnectionData GetSocketConnectionInfo(int Socket)
{
	struct ConnectionData Result;
	Result.SourceAddress = 0;
	Result.DestinationAddress = 0;

	struct sockaddr_in SourceAddr;
	int SourceAddrLength = sizeof(SourceAddr);

	struct sockaddr_in DestAddr;
	int DestAddrLength = sizeof(DestAddr);

	if (getsockname(Socket, (struct sockaddr*)&SourceAddr, &SourceAddrLength) != 0)
	{
		printf("GetSocketConnectionInfo Failed. Error: %s\n", strerror(errno));

		return Result;
	}

	Result.SourceAddress = SourceAddr.sin_addr.s_addr;

	if (getpeername(Socket, (struct sockaddr*)&DestAddr, &DestAddrLength) != 0)
	{
		printf("GetSocketConnectionInfo Failed. Error: %s\n", strerror(errno));

		return Result;
	}

	Result.DestinationAddress = DestAddr.sin_addr.s_addr;

	return Result;
}

// Credit: Richard Stevens
unsigned long CalculateChecksum(unsigned char* Data, size_t Length)
{
	long sum = 0;
	unsigned short *temp = (unsigned short *)Data;

	while (Length > 1) {
		sum += *temp++;
		if (sum & 0x80000000)
			sum = (sum & 0xFFFF) + (sum >> 16);
		Length -= 2;
	}

	if (Length)
		sum += (unsigned short) *((unsigned char *)temp);

	while (sum >> 16)
		sum = (sum & 0xFFFF) + (sum >> 16);

	return ~sum;
}