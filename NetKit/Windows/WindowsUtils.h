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

	return 0;
}

int GetMACAddress(unsigned long Address, unsigned char* Result)
{
	ULONG MACAddress[2];
	ULONG PhysicalAddressLength = 6;

	DWORD b = SendARP(htonl(Address), 0, MACAddress, &PhysicalAddressLength);

	if (b != NO_ERROR)
	{
		printf("GetMACAddress Failed.\n");

		return -1;
	}

	if (PhysicalAddressLength != 0)
	{
		BYTE* MACByteIter = (BYTE*)MACAddress;

		for (int Iter = 0; Iter < PhysicalAddressLength; ++Iter)
		{
			Result[Iter] = (unsigned char)MACByteIter[Iter];
		}
	}

	return 0;
}

char* GetDefaultGateway()
{
	PIP_ADAPTER_INFO Adapters;
	ULONG Size = sizeof(IP_ADAPTER_INFO);

	Adapters = (PIP_ADAPTER_INFO)malloc(Size);

	// Get Size Needed.
	if (GetAdaptersInfo(Adapters, &Size) == ERROR_BUFFER_OVERFLOW)
	{
		free(Adapters);

		Adapters = (PIP_ADAPTER_INFO)malloc(Size);
	}

	else
	{
		return NULL;
	}

	if (GetAdaptersInfo(Adapters, &Size) == NO_ERROR)
	{
		char* Result = (char*)malloc(16);

		// Default to 0.0.0.0
		strcpy_s(Result, 16, "0.0.0.0");

		for (PIP_ADAPTER_INFO Adapter = Adapters; Adapter; Adapter = Adapter->Next)
		{
			if (strcmp(Adapter->GatewayList.IpAddress.String, "0.0.0.0") != 0)
			{
				strncpy_s(Result, 16, Adapter->GatewayList.IpAddress.String, 16);
			}
		}

		free(Adapters);

		return Result;
	}

	else
	{
		return NULL;
	}
}