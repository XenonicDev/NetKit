/*
	Copyright (c) 2017-2018 Andrew Depke
*/
#pragma once

#include "../Platform.h"

#include <stdio.h>

struct ifaddrs* GetNetworkDevices()
{
	struct ifaddrs* Result = (struct ifaddrs*)malloc(sizeof(struct ifaddrs));

	getifaddrs(&Result);

	return Result;
}

int PrintNetworkDevices()
{
	struct ifaddrs* Addresses = GetNetworkDevices();

	printf("Network Devices -\n");

	int Iter = 1;
	for (struct ifaddrs* Address = Addresses; Address; Address = Address->ifa_next)
	{
		// Only Print AF_PACKET Interfaces.
		if (Address->ifa_addr->sa_family == AF_PACKET)
		{
			printf(" %d. %s\n", Iter, Address->ifa_name);

			++Iter;
		}
	}

	free(Addresses);

	return 0;
}

// Not Supported.
int GetMACAddress(unsigned long Address, unsigned char* Result)
{
	return 1;
}

char* GetDefaultGateway()
{
	char* Result = NULL;

	FILE* Handle;
	char Line[100];
	char* p, *c, *g, *Save;

	Handle = fopen("/proc/net/route", "r");
	while (fgets(Line, 100, Handle))
	{
		p = strtok_r(Line, " \t", &Save);
		c = strtok_r(NULL, " \t", &Save);
		g = strtok_r(NULL, " \t", &Save);

		if (p != NULL && c != NULL)
		{
			if (strcmp(c, "00000000") == 0)
			{
				if (g)
				{
					char* End;
					int GatewayRaw = strtol(g, &End, 16);

					struct sockaddr_in Gateway;
					Gateway.sin_addr.s_addr = GatewayRaw;

					Result = (char*)malloc(16);

					inet_ntop(AF_INET, (struct sockaddr*)&Gateway, Result, 16);
				}

				break;
			}
		}
	}

	fclose(Handle);

	return Result;
}