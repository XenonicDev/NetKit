/*
	Copyright (c) 2017-2018 Andrew Depke
*/
#pragma once

// TCP Pseudo Header
typedef struct
{
	unsigned int SourceAddress;
	unsigned int DestinationAddress;
	unsigned char Reserved;
	unsigned char Protocol;
	unsigned short Length;
} PSEUDO_HEADER;