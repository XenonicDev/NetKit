/*
	Copyright (c) 2017-2018 Andrew Depke
*/
#pragma once

#include "Platform.h"

#include <stdlib.h>
#include <stdio.h>

char* ErrorString(int ErrorCode)
{
#ifdef PLATFORM_WINDOWS
	char* Result = (char*)malloc(128);

	// No Error, Try WinSock Error Fetch.
	if (ErrorCode == 0)
	{
		if (WSAGetLastError() != 0)
		{
			_itoa_s(WSAGetLastError(), Result, 128, 10);
		}
	}

	else
	{
		strerror_s(Result, 128, ErrorCode);
	}

	return Result;
#else
	return strerror(ErrorCode);
#endif
}

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

char* AddressToString(unsigned long int Address)
{
	char* Result = (char*)malloc(INET_ADDRSTRLEN * sizeof(char));

	struct sockaddr_in AddressAsSockaddr;
	AddressAsSockaddr.sin_addr.s_addr = Address;

	inet_ntop(AF_INET, &AddressAsSockaddr, Result, INET_ADDRSTRLEN);

	return Result;
}

char* GetHostName()
{
	char* Result = (char*)malloc(128 * sizeof(char));

	if (gethostname(Result, 128) != 0)
	{
		char* Error = ErrorString(errno);

		printf("GetHostName Failed. Error: %s\n", Error);

#ifdef PLATFORM_WINDOWS
		free(Error);
#endif
	}

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