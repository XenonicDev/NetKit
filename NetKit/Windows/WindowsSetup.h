/*
	Copyright (c) 2017-2018 Andrew Depke
*/
#pragma once

#include "../Platform.h"

int Setup()
{
	struct WSAData Data;

	if (WSAStartup(0x0202, &Data) == SOCKET_ERROR)
	{
		return 1;
	}

	return 0;
}

void Shutdown()
{
	WSACleanup();
}