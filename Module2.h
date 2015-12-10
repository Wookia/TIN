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

extern "C" void* senderThreadWorkerDel(void*);
extern "C" void* receiverThreadWorkerDel(void*);
class Module2
{
	public:
		Module2();
		int init();
		int startThreads();
		int joinThreads();
		void* senderThreadWorker(void*);
		void* receiverThreadWorker(void*);
		
	private:
		pthread_t senderThread, receiverThread;
		sigset_t outselect, inselect;
		int nasz_socket, rc;
	
	
};

#endif
