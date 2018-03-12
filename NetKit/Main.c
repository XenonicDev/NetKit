/*
	Copyright (c) 2017-2018 Andrew Depke
*/
#include "SysUtils.h"
#include "SocketUtils.h"

#include "IP.h"
#include "TCP.h"

#ifdef PLATFORM_WINDOWS
#include "Windows/WindowsSetup.h"
#else
#include "Unix/UnixSetup.h"
#endif

int Socket;
int IsRawSocket;
int Protocol;

void FatalError()
{
	printf("\n\n-- Fatal Error Occurred, Closing --\n\n");

	system("pause");

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

	system("pause");
}

void MenuCreateSocket()
{
	char* Input = NULL;
	while (!Input)
	{
		ClearScreen();

		printf("Socket Protocol Selector -\n");
		printf("1. Raw (ETH_P_ALL)\n");
		printf("2. TCP\n");
		printf("3. UDP\n");

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
		printf("Enter Device Name: ");

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

void MenuSendPacket()
{
	MenuCreateSocket();
	MenuBindSocket();

	ShutdownSocket(Socket);

	system("pause");
}

void MenuLaunchPacketPump()
{
	MenuCreateSocket();
	MenuBindSocket();

	ShutdownSocket(Socket);

	system("pause");
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

		system("pause");

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