#pragma once

#include "Platform.h"

#include <stdlib.h>
#include <stdio.h>

char* getline()
{
	char* line = malloc(100), *linep = line;
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
			char * linen = realloc(linep, lenmax *= 2);

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

struct ConnectionData
{
	unsigned long int SourceAddress;
	unsigned long int DestinationAddress;
};

char* AddressToString(unsigned long int Address)
{
	char* Result = (char*)malloc(16 * sizeof(char));

	struct sockaddr_in AddressAsSockaddr;
	AddressAsSockaddr.sin_addr.s_addr = Address;

	inet_ntop(AF_INET, &AddressAsSockaddr, Result, 16);

	return Result;
}

char* GetHostName()
{
	char* Result = (char*)malloc(128 * sizeof(char));

	if (gethostname(Result, 128) != 0)
	{
		printf("GetHostName Failed\n");
	}

	return Result;
}

struct ConnectionData GetSocketConnectionInfo(int Socket)
{
	struct ConnectionData Result;
	Result.SourceAddress = 0;
	Result.DestinationAddress = 0;

	struct sockaddr_in SourceAddr;

	struct sockaddr_in DestAddr;

	if (getsockname(Socket, (struct sockaddr*)&SourceAddr, sizeof(SourceAddr)) != 0)
	{
		printf("GetSocketConnectionInfo Failed\n");

		return Result;
	}

	Result.SourceAddress = SourceAddr.sin_addr.s_addr;

	if (getpeername(Socket, (struct sockaddr*)&DestAddr, (size_t)sizeof(DestAddr)) != 0)
	{
		printf("GetSocketConnectionInfo Failed\n");

		return Result;
	}

	Result.DestinationAddress = DestAddr.sin_addr.s_addr;

	return Result;
}