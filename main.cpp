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
#include "Module2.h"
#include <list>
#include "server.h"
#include "SynchronizedQueue.h"

using namespace std;

SynchronizedQueue<Packet>* queuePointer;
Module2* module2Pointer;
Server* serverPointer;

void firstHandler(int signo)
{
	
}

void secondHandler(int signo)
{
	
}

void sigintHandler(int signo)
{
	if(serverPointer!=NULL)
		serverPointer->closeServer();
	if(module2Pointer!=NULL)
		module2Pointer->closeModule();
}


void init_signal_handling()
{
	struct sigaction s;
    s.sa_handler = firstHandler;
    sigemptyset(&s.sa_mask);
    s.sa_flags = 0;
    sigaction(SIGUSR1, &s, NULL);

    struct sigaction s2;
    s2.sa_handler = secondHandler;
    sigemptyset(&s2.sa_mask);
    s2.sa_flags = 0;
    sigaction(SIGUSR2, &s2, NULL);
    
    struct sigaction s3;
    s3.sa_handler = sigintHandler;
    sigemptyset(&s3.sa_mask);
    s3.sa_flags = 0;
    sigaction(SIGINT, &s3, NULL);
    
    sigset_t signalSet;
    sigset_t unblockedSignalsSet;
    sigemptyset(&signalSet);
    sigemptyset(&unblockedSignalsSet);
    sigaddset(&signalSet, SIGUSR2);
    sigaddset(&signalSet, SIGUSR1);
    //sigaddset(&unblockedSignalsSet, SIGINT);
    sigprocmask(SIG_BLOCK, &signalSet, NULL);
    //sigprocmask(SIG_UNBLOCK, &unblockedSignalsSet, NULL);
}

int main()
{
    init_signal_handling();
    SynchronizedQueue<Packet> queueToModule2;
    queuePointer = &queueToModule2;
	Module2 module2(&queueToModule2);
	module2Pointer = &module2;
	Server server(&queueToModule2);
	serverPointer = &server;
	
	module2.join();
	
	return 0;
}
