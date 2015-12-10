#ifndef packet_h_
#define packet_h_

#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <netinet/ip.h>

class Packet
{
	public:
		struct iphdr;
		int identifier, sequence;
		int replyType;
};
#endif
