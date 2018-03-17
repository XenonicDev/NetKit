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

unsigned char InterfaceMAC[6] = { 0, 0, 0, 0, 0, 0 };

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

	PrintNetworkDevices();

	Pause();
}

void MenuCreateSocket()
{
	char* Input = NULL;
	while (!Input)
	{
		ClearScreen();

		printf("Socket Protocol Selector -\n");
		printf("1. Raw\n");
		printf("2. TCP\n");
		printf("3. UDP\n");

		Input = GetInput();

		if (Input[0] != '1' && Input[0] != '2' && Input[0] != '3')
		{
			free(Input);
			Input = NULL;

			printf("\nInvalid Input, Try Again.\n");

#ifdef PLATFORM_WINDOWS
			Sleep(2000);
#else
			sleep(2);
#endif
		}
	}

	Protocol = atoi(Input);

	free(Input);

	if (Protocol == 1)
	{
#ifdef PLATFORM_WINDOWS
		Protocol = 0;  // No Need for a Protocol, This is Managed by WinPcap.
#else
		Protocol = ETH_P_ALL;
#endif
		
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
		printf("Enter Network Interface Index: ");

		Input = GetInput();

		char* DeviceName = (char*)malloc(128 * sizeof(char));

#ifdef PLATFORM_WINDOWS
		pcap_if_t* Devices = GetNetworkDevices();

		int Iter = 1;
		for (pcap_if_t* Device = Devices->next; Device; Device = Device->next)
		{
			if (atoi(Input) == Iter)
			{
				strncpy_s(DeviceName, 128, Device->name, 128);

				for (pcap_addr_t* DeviceAddress = Device->addresses; DeviceAddress; DeviceAddress = DeviceAddress->next)
				{
					if (DeviceAddress->addr->sa_family == AF_INET && DeviceAddress->addr && DeviceAddress->netmask)
					{
						GetMACAddress(ntohl(((struct sockaddr_in*)Device->addresses->addr)->sin_addr.s_addr), InterfaceMAC);

						break;
					}
				}

				break;
			}

			++Iter;
		}
#else
#endif

		if (BindSocketRaw(Socket, DeviceName, Protocol) != 0)
		{
			FatalError();
		}

		free(DeviceName);
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

	if (InterfaceMAC[0] != 0 || InterfaceMAC[1] != 0 || InterfaceMAC[2] != 0 || InterfaceMAC[3] != 0 || InterfaceMAC[4] != 0 || InterfaceMAC[5] != 0)
	{
		printf("Enter Source MAC Address (0 to Use Selected Interface '%.2X:%.2X:%.2X:%.2X:%.2X:%.2X'): ", InterfaceMAC[0], InterfaceMAC[1], InterfaceMAC[2], InterfaceMAC[3], InterfaceMAC[4], InterfaceMAC[5]);
	}

	else
	{
		printf("Enter Source MAC Address: ");
	}

	InputSource = GetInput();

	unsigned char GatewayMAC[6] = { 0, 0, 0, 0, 0, 0 };

	char* DefaultGateway = GetDefaultGateway();
	if (DefaultGateway)
	{
		unsigned long GatewayAddress;
		inet_pton(AF_INET, DefaultGateway, &GatewayAddress);

		if (GetMACAddress(ntohl(GatewayAddress), GatewayMAC) == 0)
		{
			printf("Enter Destination MAC Address (0 to Use Predicted Default Gateway '%.2X:%.2X:%.2X:%.2X:%.2X:%.2X'): ", GatewayMAC[0], GatewayMAC[1], GatewayMAC[2], GatewayMAC[3], GatewayMAC[4], GatewayMAC[5]);
		}

		else
		{
			printf("Enter Destination MAC Address: ");
		}

		free(DefaultGateway);
	}

	else
	{
		printf("Enter Destination MAC Address: ");
	}

	InputDestination = GetInput();

	ETHERNET_HEADER* Result = NULL;

	char* SourceMAC = (char*)malloc(18 * sizeof(char));
	char* DestinationMAC = (char*)malloc(18 * sizeof(char));

	if (atoi(InputSource) == 0)
	{
#ifdef PLATFORM_WINDOWS
		sprintf_s(SourceMAC, 18, "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X", InterfaceMAC[0], InterfaceMAC[1], InterfaceMAC[2], InterfaceMAC[3], InterfaceMAC[4], InterfaceMAC[5]);
#else
		sprintf(SourceMAC, "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X", InterfaceMAC[0], InterfaceMAC[1], InterfaceMAC[2], InterfaceMAC[3], InterfaceMAC[4], InterfaceMAC[5]);
#endif
	}

	else
	{
#ifdef PLATFORM_WINDOWS
		strncpy_s(SourceMAC, 18, InputSource, strlen(InputSource) - 1);
#else
		strncpy(SourceMAC, InputSource, strlen(InputSource) - 1);
#endif
	}

	if (atoi(InputDestination) == 0)
	{
#ifdef PLATFORM_WINDOWS
		sprintf_s(DestinationMAC, 18, "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X", GatewayMAC[0], GatewayMAC[1], GatewayMAC[2], GatewayMAC[3], GatewayMAC[4], GatewayMAC[5]);
#else
		sprintf(DestinationMAC, "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X", GatewayMAC[0], GatewayMAC[1], GatewayMAC[2], GatewayMAC[3], GatewayMAC[4], GatewayMAC[5]);
#endif
	}

	else
	{
#ifdef PLATFORM_WINDOWS
		strncpy_s(DestinationMAC, 18, InputDestination, strlen(InputDestination) - 1);
#else
		strncpy(DestinationMAC, InputDestination, strlen(InputDestination) - 1);
#endif
	}

	free(InputSource);
	free(InputDestination);

	Result = CreateEthernetHeader(SourceMAC, DestinationMAC, 0x0800);  // ETHERTYPE_IP (IPv4)

	free(SourceMAC);
	free(DestinationMAC);

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
		Result = CreateIPHeader(IPPROTO_TCP, ntohl(SourceAddress.sin_addr.s_addr), ntohl(DestinationAddress.sin_addr.s_addr), 0);
	}

	else
	{
		Result = CreateIPHeader(Protocol, ntohl(SourceAddress.sin_addr.s_addr), ntohl(DestinationAddress.sin_addr.s_addr), 0);
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

unsigned long MenuGetDestinationAddress()
{
	char* Input = NULL;

	printf("Enter Destination Address: ");

	Input = GetInput();

	struct sockaddr_in Address;

	inet_ntop(AF_INET, (struct sockaddr*)&Address, Input, sizeof(Address));

	free(Input);

	return Address.sin_addr.s_addr;
}

int MenuGetDestinationPort()
{
	char* Input = NULL;

	printf("Enter Destination Port: ");

	Input = GetInput();

	int Port = atoi(Input);

	free(Input);

	return Port;
}

void MenuSendPacket()
{
	MenuCreateSocket();
	MenuBindSocket();

	if (IsRawSocket == 1)
	{
		ETHERNET_HEADER* EthernetHeader = MenuEthernetHeader();
		IP_HEADER* IPHeader = MenuIPHeader();
		TCP_HEADER* TCPHeader = MenuTCPHeader();

		if (!EthernetHeader || !IPHeader || !TCPHeader)
		{
			FatalError();
		}

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
		ClearScreen();

		unsigned long Address = MenuGetDestinationAddress();
		int Port = MenuGetDestinationPort();

		if (SendPacket(Socket, ntohl(Address), Port, NULL, 0) != 0)
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