#include "Module2.h"
#include "SynchronizedQueue.h"
#include "PacketGenerator.h"

using namespace std;

Module2::Module2()
{
	nasz_socket = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (nasz_socket == -1)
	{
		perror("socket:");
		exit(1);
	}
}

int Module2::init()
{
	nasz_socket = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (nasz_socket == -1)
	{
		perror("socket:");
		exit(1);
	}
	return 0;
	
	//todomadafaka
}

int Module2::startThreads()
{
	pthread_create(&senderThread, NULL, &senderThreadWorkerDel, reinterpret_cast<void*>(this));
    pthread_create(&receiverThread, NULL, &receiverThreadWorkerDel, reinterpret_cast<void*>(this));
    return 0;
}

int Module2::joinThreads()
{
    pthread_join(senderThread, NULL);
    pthread_join(receiverThread, NULL);
    return 0;
}

void* senderThreadWorkerDel(void* delegate)
{
	return reinterpret_cast<Module2*>(delegate)->senderThreadWorker(NULL);
}

void* Module2::senderThreadWorker(void* argument)
{
	PacketGenerator packetgen;
	struct sockaddr_in addr;
	struct icmphdr* header = NULL;
	int ttl = 1;
	int* data = NULL;
	char buf[sizeof(struct icmphdr) + sizeof(int)];
	header = (struct icmphdr*)buf;
	data = (int*)(buf + sizeof(struct icmphdr));
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	inet_aton("91.198.174.192", &addr.sin_addr);
	
	for(ttl = 1; ttl <= 20; ttl++)
	{
		setsockopt(nasz_socket, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
		packetgen.generatePacket(header, data, 1, ttl);
		rc = sendto(nasz_socket,buf,sizeof(struct icmphdr) + sizeof(int),
				0, (struct sockaddr*)&addr, sizeof(addr));
		if(rc == -1)
		{
			perror("sendto:");
			exit(1);
		}
		sleep(2);
	}
	return NULL;
}

void* receiverThreadWorkerDel(void* delegate)
{
	return reinterpret_cast<Module2*>(delegate)->receiverThreadWorker(NULL);
}

void* Module2::receiverThreadWorker(void* argument)
{
	int rc;
	int offset = 28;
	char rbuf[60]; //sizeof(struct iphdr) + sizeof(struct icmp)
	struct sockaddr_in raddr;
    socklen_t raddr_len;
    struct iphdr* iphdr = NULL;
    struct icmphdr* icmphdr = NULL;
    char str[INET_ADDRSTRLEN];
    raddr_len = sizeof(raddr);
	while(1)
	{
		rc = recvfrom(nasz_socket, rbuf, sizeof(rbuf), 0, (struct sockaddr*)&raddr, &raddr_len);
		if (rc == -1) 
		{
			perror("recvfrom 2:");
			exit(1);
		}
		printf("Odberano %d bajtow\n",rc);
		iphdr = (struct iphdr*)rbuf;
		
		if (iphdr->protocol != IPPROTO_ICMP) 
		{
			fprintf(stderr, "Expected ICMP packet, got %u\n", iphdr->protocol);
			exit(1);
		}
		icmphdr = (struct icmphdr*)(rbuf + (iphdr->ihl * 4));
		printf("Dlugosc headera ip %d\n", iphdr->ihl);
		//printf("Dlugosc headera icmp %d\n", icmphdr->ihl);
		if (!(icmphdr->type == ICMP_ECHOREPLY ||  icmphdr->type == ICMP_TIME_EXCEEDED)) 
		{
			fprintf(stderr, "Expected ICMP echo-reply, got %u\n", icmphdr->type);
			exit(1);
		}
		if(icmphdr->type == ICMP_TIME_EXCEEDED)
		{
			printf("TIME EXCEEDED\n");	
			icmphdr = (struct icmphdr*)(rbuf + (iphdr->ihl * 4) + offset);
		}
		printf("Otrzymana sekwencja: %x",icmphdr->un.echo.sequence);
		printf(" Identifier %x\n", icmphdr->un.echo.id);
		cout << inet_ntop(AF_INET, &(raddr.sin_addr), str, INET_ADDRSTRLEN) << " Rodzina: " << raddr.sin_family <<" " << endl;
		memset(&raddr, 0, sizeof(raddr));
	}
	return NULL;
}
