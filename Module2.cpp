#include "Module2.h"


using namespace std;

void* managerThreadWorkerDel(void* delegate)
{
	return reinterpret_cast<Module2*>(delegate)->managerThreadWorker(NULL);
}

Module2::Module2 (SynchronizedQueue<Packet>* queueIntoM2)
{
	nasz_socket = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
	queueIntoModule = queueIntoM2;
	if (nasz_socket == -1)
	{
		perror("socket:");
		exit(1);
	}
	pthread_create(&managerThread, NULL, &managerThreadWorkerDel, reinterpret_cast<void*>(this));
}

int Module2::init(string& address, long long int taskNr, int newRetries)
{
	Module3 module3;
	tracedAddress = address;
    taskNumber = taskNr;
	retries = newRetries;
	sigemptyset(&inselect);
    sigemptyset(&outselect);
    sigaddset(&inselect, SIGUSR2);
    sigaddset(&outselect, SIGUSR1);
	return 0;
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

void Module2::join()
{
	pthread_join(managerThread,NULL);
}

void* senderThreadWorkerDel(void* delegate)
{
	return reinterpret_cast<Module2*>(delegate)->senderThreadWorker(NULL);
}

void* Module2::senderThreadWorker(void* argument)
{
	fd_set set;
	FD_ZERO(&set);
	FD_SET(0, &set);

	struct timespec timerSet;
	timerSet.tv_sec = 3;
	timerSet.tv_nsec = 0;

	struct timespec timerSet2;
	timerSet2.tv_sec = 20;
	timerSet2.tv_nsec = 0;

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
	inet_aton(tracedAddress.c_str(), &addr.sin_addr);

	int loopRetries = retries;
	for(ttl = 1; ttl <= 20; ttl++)
	{
		setsockopt(nasz_socket, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
		packetgen.generatePacket(header, data, 1, ttl);
		cout << "Wysylanie pakietu TTL " << ttl << endl;
		rc = sendto(nasz_socket,buf,sizeof(struct icmphdr) + sizeof(int),
				0, (struct sockaddr*)&addr, sizeof(addr));
		if(rc == -1)
		{
			perror("sendto:");
			exit(1);
		}
		sleep(1);
		pselect(1, &set, NULL, NULL, &timerSet, &outselect);
		if (errno == EINTR)
		{
			errno = 0;
			loopRetries = retries;
			continue;
		}
		else
		{
			if(pthread_kill(receiverThread, 0) != 0)
			{
				printf("Oho, odbieracz juz nie zyje. Wysylacz zegna!\n");
				break;
			}
			loopRetries--;
			if(loopRetries == 0)
			{
				pselect(1, &set, NULL, NULL, &timerSet2, &outselect);
				if (errno == EINTR)
				{
					errno = 0;
					loopRetries = retries;
					continue;
				}
				break;
			}
			ttl--;
		}
	}
	return NULL;
}

void* receiverThreadWorkerDel(void* delegate)
{
	return reinterpret_cast<Module2*>(delegate)->receiverThreadWorker(NULL);
}

void* Module2::receiverThreadWorker(void* argument)
{
	fd_set set2;
	FD_ZERO(&set2);
	FD_SET(nasz_socket, &set2);
	cout << "NASZ SOCKET MA WARTOSC " << nasz_socket << endl;

	struct timespec timerSet;
	timerSet.tv_sec = 20;
	timerSet.tv_nsec = 0;

	int rc;
	int offset = 28;
	int count;
	int retries = 5;
	char rbuf[60]; //sizeof(struct iphdr) + sizeof(struct icmp)
	struct sockaddr_in raddr;
    socklen_t raddr_len;
    struct iphdr* iphdr = NULL;
    struct icmphdr* icmphdr = NULL;
    char str[INET_ADDRSTRLEN];
    raddr_len = sizeof(raddr);
    result.addresses.clear();
		Traceroute traceroute;
    result.addresses.push_back(traceroute);
		result.taskNr = -1;
	while(1)
	{
		count = 1;
		count = pselect(4, &set2, NULL, NULL, &timerSet, NULL);
		if(count==0)
		{
				cout << "count = " << count << endl;
				retries--;
				if(retries==0)
					return NULL;
				FD_ZERO(&set2);
				FD_SET(nasz_socket, &set2);
				timerSet.tv_sec = 20;
				timerSet.tv_nsec = 0;
				pthread_kill(senderThread,SIGUSR2);

				continue;
		}
		retries = 5;
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
		std::string senderAddress = inet_ntop(AF_INET, &(raddr.sin_addr), str, INET_ADDRSTRLEN);
		cout << senderAddress << " Rodzina: " << raddr.sin_family <<" " << endl;
		Packet receivedPacket;

		//JEZELI CHCEMY TRZYMAC CALA STRUKTURE IPHDR TRZEBA ZROBIC JEJ KOPIOWANIE DO KLASY PACKET
		receivedPacket.identifier = icmphdr->un.echo.id;
		receivedPacket.sequence_ttl = icmphdr->un.echo.sequence;
		receivedPacket.replyType = icmphdr->type;
		receivedPacket.ip_address = senderAddress;
		cout << "ip_address:" << receivedPacket.ip_address << endl;
		result.taskNr = taskNumber;
		result.addresses.front().road.push_back(senderAddress);
		memset(&raddr, 0, sizeof(raddr));


		if(senderAddress == tracedAddress)
		{
			printf("Uff, juz po wszystkim. Odbieracz odmelodwuje sie!\n");
			return NULL;
		}

		printf("Wysylanie sygnalu\n");
		pthread_kill(senderThread,SIGUSR2);
	}
	return NULL;
}



void* Module2::managerThreadWorker(void* argument)
{
	//getAJob() //narazie petla na 3 zadanka
	for(int i=0;i<3;i++)
	{
		Packet test = queueIntoModule->pop();
		//do the traceroute
        std::cout<<"kura"<<test.identifier<<std::endl;
		init(test.ip_address, test.identifier, 4);
		startThreads();
		joinThreads();
		//get the results back
        std::cout<<"KOŃ"<<std::endl;
		module3.saveData(result);
		//back to 1
	}
	return NULL;
}
