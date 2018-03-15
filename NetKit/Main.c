/*
	Copyright (c) 2017-2018 Andrew Depke
*/
#include "SysUtils.h"
#include "SocketUtils.h"

#include "Ethernet.h"
#include "IP.h"
#include "TCP.h"

#ifdef PLATFORM_WINDOWS
#include "Windows/WindowsSetup.h"
#include "Windows/WindowsUtils.h"
#else
#include "Unix/UnixSetup.h"
#include "Unix/UnixUtils.h"
#endif

int Socket;
int IsRawSocket;
int Protocol;

void FatalError()
{
	printf("\n\n-- Fatal Error Occurred, Closing --\n\n");

	Pause();

	exit(1);
}

void PrintHeader()
{
	printf("- NetKit v0.3 -\n");
	printf("Copyright (c) 2018 Andrew Depke\n");
}

void SystemInfo()
{
	ClearScreen();

	char* HostName = GetHostName();

	printf("Host Name: %s\n", HostName);

	free(HostName);

	Pause();
}

void MenuCreateSocket()
{
	char* Input = NULL;
	while (!Input)
	{
		ClearScreen();

		printf("Socket Protocol Selector -\n");
#ifdef PLATFORM_WINDOWS
		printf("1. TCP\n");
		printf("2. UDP\n");
#else
		printf("1. Raw\n");
		printf("2. TCP\n");
		printf("3. UDP\n");
#endif

		Input = GetInput();

		if (Input[0] != '1' && Input[0] != '2' && Input[0] != '3')
		{
			free(Input);
			Input = NULL;

			printf("\nInvalid Input, Try Again.\n");

			sleep(2);
		}
	}

	Protocol = atoi(Input);

	free(Input);

#ifdef PLATFORM_WINDOWS
	if (Protocol == 1)
	{
		Protocol = IPPROTO_TCP;
		IsRawSocket = 0;
	}

	else
	{
		Protocol = IPPROTO_UDP;
		IsRawSocket = 0;
	}
#else
	if (Protocol == 1)
	{
		Protocol = ETH_P_ALL;
		IsRawSocket = 1;
	}

	else if (Protocol == 2)
	{
		Protocol = IPPROTO_TCP;
		IsRawSocket = 0;
	}

	else
	{
		Protocol = IPPROTO_UDP;
		IsRawSocket = 0;
	}
#endif

	if (IsRawSocket == 1)
	{
		Socket = CreateSocketRaw(Protocol, 1);
	}

	else
	{
		Socket = CreateSocket(Protocol);
	}

	if (Socket == -1)
	{
		FatalError();
	}
}

void MenuBindSocket()
{
	char* Input = NULL;

	ClearScreen();

	printf("Socket Binding Configuration -\n");

	if (IsRawSocket == 1)
	{
		printf("Enter Network Interface Name: ");

		Input = GetInput();

		if (BindSocketRaw(Socket, Input, Protocol) != 0)
		{
			FatalError();
		}
	}

	else
	{
		printf("Enter Local IPv4 Address (0 for Automatic Assignment): ");

		Input = GetInput();

		unsigned long Address = 0;

		if (Input[0] != '0')
		{
			struct sockaddr_in SockAddr;

			inet_pton(AF_INET, Input, &SockAddr.sin_addr);

			Address = ntohl(SockAddr.sin_addr.s_addr);
		}

		printf("Enter Local Port (0 for Automatic Assignment): ");

		Input = GetInput();

		int Port = 0;

		if (BindSocket(Socket, Address, Port) != 0)
		{
			FatalError();
		}
	}

	free(Input);
}

ETHERNET_HEADER* MenuEthernetHeader()
{
	char* InputSource = NULL;
	char* InputDestination = NULL;

	ClearScreen();

	printf("Ethernet Header Creator -\n");
	printf("Enter Source MAC Address (0 for Automatic Assignment): ");

	InputSource = GetInput();

	char* DefaultGateway = GetDefaultGateway();
	if (DefaultGateway)
	{
		printf("Predicted Default Gateway: %s\n", DefaultGateway);

		free(DefaultGateway);

		printf("Enter Destination MAC Address (0 to Use Predicted Default Gateway): ");
	}

	else
	{
		printf("Could Not Determine Default Gateway.\n");
		printf("Enter Destination MAC Address: ");
	}

	InputDestination = GetInput();

	ETHERNET_HEADER* Result = CreateEthernetHeader(InputSource, InputDestination, 0x0800);  // ETHERTYPE_IP (IPv4)

	free(InputSource);
	free(InputDestination);

	return Result;
}

IP_HEADER* MenuIPHeader()
{
	char* InputSource = NULL;
	char* InputDestination = NULL;

	ClearScreen();

	printf("IP Header Creator -\n");
	printf("Enter Source IP Address: ");

	InputSource = GetInput();

	printf("\nEnter Destination IP Address: ");

	InputDestination = GetInput();

	struct sockaddr_in SourceAddress;
	struct sockaddr_in DestinationAddress;

	inet_ntop(AF_INET, (struct sockaddr*)&SourceAddress.sin_addr.s_addr, InputSource, 16);
	inet_ntop(AF_INET, (struct sockaddr*)&DestinationAddress.sin_addr.s_addr, InputDestination, 16);

	free(InputSource);
	free(InputDestination);

	IP_HEADER* Result = NULL;

	if (IsRawSocket == 1)
	{
		IP_HEADER* Result = CreateIPHeader(IPPROTO_TCP, ntohl(SourceAddress.sin_addr.s_addr), ntohl(DestinationAddress.sin_addr.s_addr), 0);
	}

	else
	{
		IP_HEADER* Result = CreateIPHeader(Protocol, ntohl(SourceAddress.sin_addr.s_addr), ntohl(DestinationAddress.sin_addr.s_addr), 0);
	}

	return Result;
}

TCP_HEADER* MenuTCPHeader()
{
	char* InputSource = NULL;
	char* InputDestination = NULL;

	ClearScreen();

	printf("TCP Header Creator -\n");
	printf("Enter Source Port: ");

	InputSource = GetInput();

	printf("\nEnter Destination Port: ");

	InputDestination = GetInput();

	TCP_HEADER* Result = CreateTCPHeader(atoi(InputSource), atoi(InputDestination), 0, 0);

	return Result;
}

void MenuSendPacket()
{
	MenuCreateSocket();
	MenuBindSocket();

	ETHERNET_HEADER* EthernetHeader = NULL;
	IP_HEADER* IPHeader = NULL;
	TCP_HEADER* TCPHeader = NULL;

	if (IsRawSocket == 1)
	{
		EthernetHeader = MenuEthernetHeader();
	}

	IPHeader = MenuIPHeader();
	TCPHeader = MenuTCPHeader();

	if (IsRawSocket == 1)
	{
		Packet* Pack = CreateTCPPacket(EthernetHeader, IPHeader, TCPHeader, NULL, 0);

		if (SendPacketRaw(Socket, Pack) != 0)
		{
			printf("Failed to Send Packet\n");
		}

		CleanupPacket(Pack);
		free(Pack);
	}

	else
	{
		if (SendPacket(Socket, ntohl(IPHeader->daddr), ntohl(TCPHeader->dest), NULL, 0) != 0)
		{
			printf("Failed to Send Packet\n");
		}
	}

	ShutdownSocket(Socket);

	Pause();
}

void MenuLaunchPacketPump()
{
	MenuCreateSocket();
	MenuBindSocket();

	ShutdownSocket(Socket);

	Pause();
}

void PrintMenu()
{
	PrintHeader();
	printf("\n-------------------------------\n");
	printf("1. System Information\n");
	printf("2. Send Packet...\n");
	printf("3. Launch Packet Pump...\n");
}

int main(int ArgC, char** ArgV)
{
	PrintMenu();

	if (Setup() != 0)
	{
		printf("\n Setup Failed. Closing.\n");

		Pause();

		return -1;
	}

	char* Input;
	size_t InputLength = 32;

	while (1)
	{
		Input = GetInput();

		if (!Input)
		{
			ClearScreen();

			PrintMenu();

			printf("\nError Reading Input. Try Again.\n");
		}

		else
		{
			if (Input[0] == '1')
			{
				SystemInfo();

				break;
			}

			else if (Input[0] == '2')
			{
				MenuSendPacket();

				break;
			}

			else if (Input[0] == '3')
			{
				MenuLaunchPacketPump();

				break;
			}

			else
			{
				ClearScreen();

				PrintMenu();

				printf("\nInput Not Recognized. Try Again.\n");
			}
		}
	}

	free(Input);

	Shutdown();

	return 0;
}