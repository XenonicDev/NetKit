#pragma once

#include "../Platform.h"

#define HAVE_REMOTE

#include "pcap/pcap.h"

char* GetDefaultGateway()
{
	PIP_ADAPTER_INFO Adapter;
	ULONG Size = sizeof(Adapter);

	Adapter = (PIP_ADAPTER_INFO)malloc(Size);

	if (GetAdaptersInfo(Adapter, &Size) == NO_ERROR)
	{
		char* Result = (char*)malloc(16);

		strncpy_s(Result, 16, Adapter->GatewayList.IpAddress.String, 16);

		free(Adapter);

		return Result;
	}

	else
	{
		return NULL;
	}
}