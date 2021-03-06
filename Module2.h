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
#include <linux/types.h>
#include <netinet/ip_icmp.h>
#include <sys/stat.h>
#include "errno.h"
#include <arpa/inet.h>
#include <iostream>
#include "PacketGenerator.h"
#include "Packet.h"
#include "SynchronizedQueue.h"
#include "module3.h"
#include <string>
#include <list>
#include <fstream>
#include "Params.h"
#include <fcntl.h>
#include <map>
#include <iterator>

extern "C" void* senderThreadWorkerDel(void*);
extern "C" void* receiverThreadWorkerDel(void*);
class Module2
{
	public:
		Module2();
		void closeModule();
		Module2 (SynchronizedQueue<Packet>* queueIntoM2, Params* params, Module3* module3);
		int init(std::string& address, long long int taskNr, int newRetries);
		int startThreads();
		int joinThreads();
		void join();
		void* senderThreadWorker(void*);
		void* receiverThreadWorker(void*);
		void* managerThreadWorker(void*);
		sigset_t outselect, inselect;

	private:
		SynchronizedQueue<Packet>* queueIntoModule;
		Result result;
		Module3* module3;
		pthread_t senderThread, receiverThread, managerThread;
		sem_t senderSem, receiverSem;
		std::string tracedAddress;
        long long int taskNumber;
		int nasz_socket, rc;
		int retries;
		std::ofstream module2Output;
        int max_ttl;
        int max_packets_per_ttl;
        int timeout;
        int freq;
				int identifier;
};

#endif
