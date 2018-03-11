/*
	Copyright (c) 2017-2018 Andrew Depke
*/
#pragma once

// TCP Pseudo Header
struct PSEUDO_HEADER
{
	unsigned int SourceAddress;
	unsigned int DestinationAddress;
	unsigned char Reserved;
	unsigned char Protocol;
	unsigned short Length;
};