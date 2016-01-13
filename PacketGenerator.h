#ifndef packet_generator_h_
#define packet_generator_h_


class PacketGenerator
{
	public:
		void generatePacket(struct icmphdr*, int*, int, int);
	private:
		unsigned short in_cksum(unsigned short*,int);
};


#endif
