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
#include "Packet.h"
#include "SynchronizedQueue.h"


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
void test()
{
	

	std::list<Packet> odebrane;
	SynchronizedQueue<Packet> queueToModule2;
	SynchronizedQueue<std::list<Packet>> queueFromModule2;
	Module2 module2(&queueToModule2,&queueFromModule2);
	Packet zadanie1,zadanie2,zadanie3;
	zadanie1.ip_address = "8.8.8.8";
	zadanie2.ip_address = "216.58.209.67";
	zadanie3.ip_address = "212.77.98.9";
	queueToModule2.push(zadanie1);
	
	odebrane = queueFromModule2.pop();
	for(Packet pack:odebrane)
		{
			cout << "sciezka: " << pack.ip_address << " ttl " << pack.sequence_ttl << endl;
		}
		cout << "koniec zadania" << endl;
		queueToModule2.push(zadanie2);
	odebrane = queueFromModule2.pop();
	for(Packet pack:odebrane)
		{
			cout << "sciezka: " << pack.ip_address << " ttl " << pack.sequence_ttl << endl;
		}
		cout << "koniec zadania" << endl;
		queueToModule2.push(zadanie3);
	odebrane = queueFromModule2.pop();
	for(Packet pack:odebrane)
		{
			cout << "sciezka: " << pack.ip_address << " ttl " << pack.sequence_ttl << endl;
		}
		cout << "koniec zadania" << endl;
	module2.join();
	
	
}

int main()
{

    init_signal_handling();
    test();
	  

}
    
    
    
