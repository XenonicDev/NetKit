#pragma once

#include "../Platform.h"

#define HAVE_REMOTE

#include "pcap/pcap.h"

int PrintNetworkDevices()
{
	char ErrorBuffer[PCAP_ERRBUF_SIZE + 1];

	pcap_if_t* Devices, *Device;

	if (pcap_findalldevs_ex("rpcap://", NULL, &Devices, ErrorBuffer) == -1)
	{
		printf("GetNetworkDevices Failed. Error: %s\n", ErrorBuffer);

		return -1;
	}

	printf("Network Devices -\n");

	for (Device = Devices; Device; Device = Device->next)
	{
		printf("  %s\n", Device->name);
	}

	return 0;
}

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