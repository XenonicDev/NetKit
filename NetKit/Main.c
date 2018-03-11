#include "SysUtils.h"
#include "SocketUtils.h"

#include "IP.h"
#include "TCP.h"

#ifdef PLATFORM_WINDOWS
#include "Windows/WindowsSetup.h"
#else
#include "Unix/UnixSetup.h"
#endif

void PrintHeader()
{
	printf("- NetKit v0.1 -\n");
	printf("Copyright (c) 2018 Andrew Depke\n");
}

void SystemInfo()
{
	char* HostName = GetHostName();

	printf("Host Name: %s\n", HostName);

	free(HostName);

	system("pause");
}

void TCPSYNPacketInj()
{
	int Socket = CreateSocket(IPPROTO_TCP);
	if (Socket == -1)
	{
		printf("Socket Creation Error, Closing\n");
		system("pause");

		return;
	}

	char* Input = NULL;

	int PacketCount = 0;

	while (PacketCount < 1)
	{
		printf("Enter Packet Count: ");
		Input = getline();
		if (!Input)
		{
			printf("\nInput Error, Closing\n");
			ShutdownSocket(Socket);
			system("pause");

			return;
		}

		PacketCount = atoi(Input);

		free(Input);

		if (PacketCount < 1)
		{
			printf("\nPacket Count Must Be At Least 1\n");
		}
	}

	printf("Enter Binding Address: ");
	Input = getline();
	if (!Input)
	{
		printf("\nInput Error, Closing\n");
		ShutdownSocket(Socket);
		system("pause");

		return;
	}

	struct sockaddr_in SourceAddress;

	inet_pton(AF_INET, Input, &SourceAddress.sin_addr);

	if (SourceAddress.sin_addr.s_addr == 0)
	{
		printf("\nAddress Failed to Parse, Closing\n");
		ShutdownSocket(Socket);
		system("pause");

		return;
	}

	free(Input);

	printf("\nEnter Binding Port: ");
	Input = getline();
	if (!Input)
	{
		printf("\nInput Error, Closing\n");
		ShutdownSocket(Socket);
		system("pause");

		return;
	}

	int SourcePort = atoi(Input);

	free(Input);

	if (SourcePort == 0)
	{
		printf("\nPort Failed to Parse, Closing\n");
		ShutdownSocket(Socket);
		system("pause");

		return;
	}

	BindSocket(Socket, SourceAddress.sin_addr.s_addr, SourcePort);

	printf("Enter Destination Address: ");
	Input = getline();
	if (!Input)
	{
		printf("\nInput Error, Closing\n");
		ShutdownSocket(Socket);
		system("pause");

		return;
	}

	struct sockaddr_in DestinationAddress;

	inet_pton(AF_INET, Input, &DestinationAddress);

	if (DestinationAddress.sin_addr.s_addr == 0)
	{
		printf("\nAddress Failed to Parse, Closing\n");
		ShutdownSocket(Socket);
		system("pause");

		return;
	}

	printf("\nEnter Destination Port: ");
	Input = getline();
	if (!Input)
	{
		printf("\nInput Error, Closing\n");
		ShutdownSocket(Socket);
		system("pause");

		return;
	}

	int DestinationPort = atoi(Input);

	free(Input);

	if (DestinationPort == 0)
	{
		printf("\nPort Failed to Parse, Closing\n");
		ShutdownSocket(Socket);
		system("pause");

		return;
	}

	struct IP_HEADER* IPHeader = CreateIPHeader(IPPROTO_TCP, SourceAddress.sin_addr.s_addr, DestinationAddress.sin_addr.s_addr, 0);
	struct TCP_HEADER* TCPHeader = CreateTCPHeader(SourcePort, DestinationPort, 0, 0);

	struct Packet* FinalPacket = CreateTCPPacket(IPHeader, TCPHeader, NULL, 0);

	for (int Iter = 0; Iter < PacketCount; ++Iter)
	{
		SendPacket(Socket, FinalPacket);
	}

	free(FinalPacket);
	free(TCPHeader);
	free(IPHeader);

	ShutdownSocket(Socket);
}

void PrintMenu()
{
	PrintHeader();
	printf("\n-------------------------------\n");
	printf("1. System Information\n");
	printf("2. TCP SYN Packet Injector\n");
}

int main(int ArgC, char** ArgV)
{
	PrintMenu();

	if (Setup() != 0)
	{
		printf("\n Setup Failed. Closing.\n");

		system("pause");

		return -1;
	}

	char* Input;
	size_t InputLength = 32;

	SSIZE_T Read = 0;

	while (1)
	{
		Input = getline();

		if (!Input)
		{
			system("cls");

			PrintMenu();

			printf("\nError Reading Input. Try Again.\n");
		}

		else
		{
			if (Input[0] == '1')
			{
				system("cls");

				SystemInfo();

				break;
			}

			else if (Input[0] == '2')
			{
				system("cls");

				TCPSYNPacketInj();

				break;
			}

			else
			{
				system("cls");

				PrintMenu();

				printf("\nInput Not Recognized. Try Again.\n");
			}
		}
	}

	free(Input);

	Shutdown();

	return 0;
}