#include "SysUtils.h"
#include "SocketUtils.h"

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

void TCPPacketInj()
{
	int Socket = CreateSocket(IPPROTO_TCP);
	if (Socket == -1)
	{
		printf("Socket Creation Error, Closing\n");
		system("pause");

		return;
	}

	printf("Enter Binding Address: ");
	char* Input = getline();
	if (!Input)
	{
		printf("\nInput Error, Closing\n");
		system("pause");

		return;
	}

	struct sockaddr_in Address;

	inet_pton(AF_INET, Input, &Address.sin_addr);

	if (Address.sin_addr.s_addr == 0)
	{
		printf("\nAddress Failed to Parse, Closing\n");
		system("pause");

		return;
	}

	free(Input);

	printf("\nEnter Binding Port: ");
	Input = getline();
	if (!Input)
	{
		printf("\nInput Error, Closing\n");
		system("pause");

		return;
	}

	if (atoi(Input) == 0)
	{
		printf("\nPort Failed to Parse, Closing\n");
		system("pause");

		return;
	}

	BindSocket(Socket, Address.sin_addr.s_addr, atoi(Input));

	free(Input);

	ShutdownSocket(Socket);
}

void PrintMenu()
{
	PrintHeader();
	printf("\n-------------------------------\n");
	printf("1. System Information\n");
	printf("2. TCP Packet Injector\n");
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

				TCPPacketInj();

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