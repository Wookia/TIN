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
#include "Params.h"

using namespace std;

SynchronizedQueue<Packet>* queuePointer;
Module3* module3Pointer;
Module2* module2Pointer;
Server* serverPointer;
Params* paramsPointer;

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
    sigemptyset(&signalSet);
    sigaddset(&signalSet, SIGUSR2);
    sigaddset(&signalSet, SIGUSR1);
    sigprocmask(SIG_BLOCK, &signalSet, NULL);
}

int main(int argc, char** argv)
{
    std::string configName;
    Params* paramsPointer;
    if(argc>=2)
    {
        configName = std::string(argv[1]);
		paramsPointer = new Params(configName);
        std::cout<<"Plik konfiguracyjny: "<<configName<<std::endl;
    }
    else
    {
		paramsPointer = new Params();
        std::cout<<"Domyslny plik konfiguracyjny"<<std::endl;
    }
    init_signal_handling();
    SynchronizedQueue<Packet> queueToModule2;
    Module3* module3;
    if (paramsPointer->repo_path == "")
        module3 = new Module3();
    else
        module3 = new Module3(paramsPointer->repo_path);
    queuePointer = &queueToModule2;
	Module2 module2(&queueToModule2, paramsPointer, module3);
	module2Pointer = &module2;
	Server server(&queueToModule2, paramsPointer, module3);
	serverPointer = &server;
	
	module2.join();
	printf("Wszystko zamknelo sie poprawnie!\n");
	delete(module3);
	delete(paramsPointer);
	return 0;
}
