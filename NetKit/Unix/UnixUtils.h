/*
	Copyright (c) 2017-2018 Andrew Depke
*/
#pragma once

#include "../Platform.h"

#include <stdio.h>

int PrintNetworkDevices()
{
	return 0;
}

int GetMACAddress(unsigned long Address, unsigned char* Result)
{
	return 0;
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