#pragma once

#include "../Platform.h"

#define HAVE_REMOTE

#include "pcap/pcap.h"

// Grab a Linked List of Interfaces.
pcap_if_t* GetNetworkDevices()
{
	pcap_if_t* Result = (pcap_if_t*)malloc(sizeof(pcap_if_t));

	char ErrorBuffer[PCAP_ERRBUF_SIZE + 1];

	if (pcap_findalldevs_ex("rpcap://", NULL, Result, ErrorBuffer) == -1)
	{
		printf("GetNetworkDevices Failed. Error: %s\n", ErrorBuffer);

		return NULL;
	}

	return Result;
}

int PrintNetworkDevices()
{
	pcap_if_t* Devices = GetNetworkDevices();

	if (Devices == NULL)
	{
		return -1;
	}

	pcap_if_t* Device;

	printf("Network Devices -\n");
	
	int Iter = 1;
	for (Device = Devices->next; Device; Device = Device->next)
	{
		printf(" %d. %s | %.30s...\n", Iter, Device->name, Device->description);

		++Iter;
	}

	free(Devices);
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