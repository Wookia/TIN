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
		int sequence_ttl;
        long long int identifier;
		int replyType;
        bool isLast;
        
		std::string ip_address;
		Packet& operator= (Packet copiedPacket)
		{
			this->ipheader = copiedPacket.ipheader;
			this->identifier = copiedPacket.identifier;
			this->replyType = copiedPacket.replyType;
			this->ip_address = copiedPacket.ip_address;
			
			return *this;
		}
};
#endif
