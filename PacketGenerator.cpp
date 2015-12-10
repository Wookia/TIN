#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "pthread.h"
#include "signal.h"
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <sys/stat.h>
#include "errno.h"
#include <arpa/inet.h>
#include <iostream>

#include "PacketGenerator.h"

	void PacketGenerator::generatePacket(struct icmphdr *header, int* data, int id, int ttl)
	{
		memset(header, 0, sizeof(struct icmphdr));
		memset(data, 0, sizeof(int));
		header->type = ICMP_ECHO;
		header->code = 0;
		header->checksum = 0;
		header->un.echo.id = id;
		header->un.echo.sequence = ttl;
		
		*data = time(NULL);	
		
		header->checksum = in_cksum((unsigned short*)header, sizeof(struct icmphdr) + sizeof(int));	
	}
	
	unsigned short PacketGenerator::in_cksum(unsigned short *addr,int len)
	{
		register int sum = 0;
		u_short answer = 0;
		register u_short *w = addr;
		register int nleft = len;

		/*
		* Our algorithm is simple, using a 32 bit accumulator (sum), we add
		* sequential 16 bit words to it, and at the end, fold back all the
		* carry bits from the top 16 bits into the lower 16 bits.
		*/
		while (nleft > 1)  
		{
			sum += *w++;
			nleft -= 2;
		}

		/* mop up an odd byte, if necessary */
		if (nleft == 1) 
		{
			*(u_char *)(&answer) = *(u_char *)w ;
			sum += answer;
		}

		/* add back carry outs from top 16 bits to low 16 bits */
		sum = (sum >> 16) + (sum & 0xffff);     /* add hi 16 to low 16 */
		sum += (sum >> 16);                     /* add carry */
		answer = ~sum;                          /* truncate to 16 bits */
		return(answer);
	}
