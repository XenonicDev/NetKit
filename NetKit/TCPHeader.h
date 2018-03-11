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
	unsigned short res1 : 4;
	unsigned short doff : 4;
	unsigned short fin : 1;
	unsigned short syn : 1;
	unsigned short rst : 1;
	unsigned short psh : 1;
	unsigned short ack : 1;
	unsigned short urg : 1;
	unsigned short ece : 1;
	unsigned short cwr : 1;
	unsigned short window;  // Window Size
	unsigned short check;  // Checksum
	unsigned short urg_ptr;  // Urgent Pointer
};
#endif