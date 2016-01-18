#include "Module2.h"


using namespace std;

void* managerThreadWorkerDel(void* delegate)
{
	return reinterpret_cast<Module2*>(delegate)->managerThreadWorker(NULL);
}

Module2::Module2 (SynchronizedQueue<Packet>* queueIntoM2, Params* params, Module3* module3Pointer)
{
    max_ttl = params->max_ttl;
    max_packets_per_ttl = params->max_packets_per_ttl;
    timeout = params->timeout;
    freq = params->freq;
    module3 = module3Pointer;
    sem_init(&senderSem, 0, 0);
	sem_init(&receiverSem, 0, 0);
	nasz_socket = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
	queueIntoModule = queueIntoM2;
	if (nasz_socket == -1)
	{
		perror("socket:");
		exit(1);
	}
	module2Output.open("module2Output.txt");
	if(!module2Output.is_open()) {
		cout << "UWAGA WYJSCIE MODULU 2 NIE OTWARTE" << endl;
	}
	pthread_create(&managerThread, NULL, &managerThreadWorkerDel, reinterpret_cast<void*>(this));
}

void Module2::closeModule()
{
	module2Output.close();
	int test = 0;
	sem_getvalue(&senderSem,&test);
	if(test != 0) pthread_cancel(senderThread);
	
	sem_getvalue(&receiverSem,&test);
	if(test != 0) pthread_cancel(receiverThread);
	
	sem_destroy(&senderSem);
	sem_destroy(&receiverSem);
	if(pthread_kill(managerThread, 0) == 0) pthread_cancel(managerThread);
	close(nasz_socket);
}

int Module2::init(string& address, long long int taskNr, int newRetries)
{
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
	sem_post(&senderSem);
	fd_set set;
	FD_ZERO(&set);
	FD_SET(0, &set);

	struct timespec timerSet;
	timerSet.tv_sec = freq;
	timerSet.tv_nsec = 0;

	struct timespec timerSet2;
	timerSet2.tv_sec = timeout;
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
	for(ttl = 1; ttl <= max_ttl; ttl++)
	{
		setsockopt(nasz_socket, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
		packetgen.generatePacket(header, data, 1, ttl);
		module2Output << "Wysylanie pakietu TTL " << ttl << endl;
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
				module2Output << "Oho, odbieracz juz nie zyje. Wysylacz zegna!" << endl;
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
	sem_wait(&senderSem);
	return NULL;
}

void* receiverThreadWorkerDel(void* delegate)
{
	return reinterpret_cast<Module2*>(delegate)->receiverThreadWorker(NULL);
}

void* Module2::receiverThreadWorker(void* argument)
{
	sem_post(&receiverSem);
	fd_set set2;
	FD_ZERO(&set2);
	FD_SET(nasz_socket, &set2);
	module2Output << "ID SOCKETU: " << nasz_socket << endl;

	struct timespec timerSet;
	timerSet.tv_sec = timeout;
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
				module2Output << "count = " << count << endl;
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


		module2Output << "Odebrano "<< rc << " bajtow"  << endl;
		iphdr = (struct iphdr*)rbuf;

		if (iphdr->protocol != IPPROTO_ICMP)
		{
			fprintf(stderr, "Expected ICMP packet, got %u\n", iphdr->protocol);
			module2Output << "Expected ICMP packet, got " << iphdr->protocol << endl;
			exit(1);
		}
		icmphdr = (struct icmphdr*)(rbuf + (iphdr->ihl * 4));
		module2Output << "Dlugosc headera ip "<< iphdr->ihl << endl;
		//printf("Dlugosc headera icmp %d\n", icmphdr->ihl);
		if (!(icmphdr->type == ICMP_ECHOREPLY ||  icmphdr->type == ICMP_TIME_EXCEEDED))
		{
			fprintf(stderr, "Expected ICMP echo-reply, got %u\n", icmphdr->type);
			module2Output << "Expected ICMP echo-reply, got " << icmphdr->type << endl;
			exit(1);
		}
		if(icmphdr->type == ICMP_TIME_EXCEEDED)
		{
			module2Output << "TIME EXCEEDED" << endl;
			icmphdr = (struct icmphdr*)(rbuf + (iphdr->ihl * 4) + offset);
		}
		module2Output << "Otrzymana sekwencja:" << icmphdr->un.echo.sequence<< endl;
		module2Output << "Identyfikator: " << icmphdr->un.echo.id<< endl;
		std::string senderAddress = inet_ntop(AF_INET, &(raddr.sin_addr), str, INET_ADDRSTRLEN);
		module2Output << senderAddress << " Rodzina: " << raddr.sin_family <<" " << endl;
		Packet receivedPacket;

		//JEZELI CHCEMY TRZYMAC CALA STRUKTURE IPHDR TRZEBA ZROBIC JEJ KOPIOWANIE DO KLASY PACKET
		receivedPacket.identifier = icmphdr->un.echo.id;
		receivedPacket.sequence_ttl = icmphdr->un.echo.sequence;
		receivedPacket.replyType = icmphdr->type;
		receivedPacket.ip_address = senderAddress;
		module2Output << "ip_address:" << receivedPacket.ip_address << endl;
		result.taskNr = taskNumber;
		result.addresses.front().road.push_back(senderAddress);
		memset(&raddr, 0, sizeof(raddr));


		if(senderAddress == tracedAddress)
		{
			module2Output << "Uff, juz po wszystkim. Odbieracz odmelodwuje sie!" << endl;
			sem_wait(&receiverSem);
			return NULL;
		}

		module2Output << "Wysylanie sygnalu!" << endl;
		pthread_kill(senderThread,SIGUSR2);
	}
	sem_wait(&receiverSem);
	return NULL;
}



void* Module2::managerThreadWorker(void* argument)
{
	//getAJob()
	while(true)
	{
		Packet test = queueIntoModule->pop();
		//do the traceroute
		init(test.ip_address, test.identifier, max_packets_per_ttl);
		startThreads();
		joinThreads();
		//get the results back
        result.isLast = test.isLast;
		module3->saveData(result);
		//back to 1
	}
	return NULL;
}
