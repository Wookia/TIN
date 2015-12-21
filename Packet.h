#ifndef packet_h_
#define packet_h_

#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string>

class Packet
{
	public:
		struct iphdr ipheader;
		int identifier, sequence_ttl;
		int replyType;
		std::string ip_address;
};
#endif
