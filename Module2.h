#ifndef module_2_h_
#define module_2_h_

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
#include "SynchronizedQueue.h"
#include "PacketGenerator.h"
#include "Packet.h"
#include <string>
#include <list>

extern "C" void* senderThreadWorkerDel(void*);
extern "C" void* receiverThreadWorkerDel(void*);
class Module2
{
	public:
		Module2();
		int init(std::string& address, int newRetries);
		int startThreads();
		int joinThreads();
		void* senderThreadWorker(void*);
		void* receiverThreadWorker(void*);
		void* managerThreadWorker(void*);
		sigset_t outselect, inselect;
		
	private:
		//SynchronizedQueue 
		std::list<Packet> traceroutePath;
		pthread_t senderThread, receiverThread, managerThread;
		std::string tracedAddress;
		int nasz_socket, rc;
		int retries;
};

#endif