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


using namespace std;

pthread_t sendingThread, receivingThread;	//test thread
pthread_t senderThread, receiverThread;	//'legit' threads
//obstawiam w tej chwili Radek, ze to pewnie ma robic brak obslugi sygnalow. albo i nie. kto wie.
void sigterm(int signo) 
{
}

void secondHandler(int signo)
{
}


void init_signal_handling()
{
	struct sigaction s;
    s.sa_handler = sigterm;
    sigemptyset(&s.sa_mask);
    s.sa_flags = 0;
    sigaction(SIGUSR1, &s, NULL);
    
    struct sigaction s2;
    s2.sa_handler = secondHandler;
    sigemptyset(&s2.sa_mask);
    s2.sa_flags = 0;
    sigaction(SIGUSR2, &s2, NULL);
    
    sigset_t signalSet;
    sigemptyset(&signalSet);
    sigaddset(&signalSet, SIGUSR2);
    sigaddset(&signalSet, SIGUSR1);
    sigprocmask(SIG_BLOCK, &signalSet, NULL);
}

int main()
{

    init_signal_handling();
    Module2 module2;
    std::string address = "212.77.98.9";
    int retries = 4;
	module2.init(address, retries);
	module2.startThreads();
	module2.joinThreads();
	  

}
    
    
    
