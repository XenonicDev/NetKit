#pragma once

#include "Platform.h"

#ifdef PLATFORM_UNIX
typedef struct tcphdr TCP_HEADER;
#else
struct TCP_HEADER
{
	unsigned short source;  // Source Port
	unsigned short dest;  // Destination Port
	unsigned int seq;  // Sequence Number
	unsigned int ack_seq;  // Acknowledgment Number
	unsigned short res1 : 4;  // Reserved Part 1
	unsigned short doff : 4;  // Data Offset
	unsigned short fin : 1;  // Finish Flag
	unsigned short syn : 1;  // Synchronize Flag
	unsigned short rst : 1;  // Reset Flag
	unsigned short psh : 1;  // Push Flag
	unsigned short ack : 1;  // Acknowledgment Flag
	unsigned short urg : 1;  // Urgent Flag
	unsigned short ece : 1;  // ECN-Echo Flag
	unsigned short cwr : 1;  // Congestion Window Reduced Flag
	unsigned short window;  // Window Size
	unsigned short check;  // Checksum
	unsigned short urg_ptr;  // Urgent Pointer
};
#endif