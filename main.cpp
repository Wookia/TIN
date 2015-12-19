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

///TODO: most likely need to set sigprocmask in one place only!

using namespace std;

pthread_t sendingThread, receivingThread;	//test thread
pthread_t senderThread, receiverThread;	//'legit' threads
sigset_t outselect, inselect;
//ENGLISH XDDD
int nasz_socket, rc;

unsigned short in_cksum(unsigned short *addr,int len)
{
	register int sum = 0;
    u_short answer = 0;
    register u_short *w = addr;
    register int nleft = len;

    /*
    * Our algorithm is simple, using a 32 bit accumulator (sum), we add
    * sequential 16 bit words to it, and at the end, fold back all the
    * carry bits from the top 16 bits into the lower 16 bits.
    */
    while (nleft > 1)  
    {
		sum += *w++;
		nleft -= 2;
    }

	/* mop up an odd byte, if necessary */
	if (nleft == 1) 
	{
		*(u_char *)(&answer) = *(u_char *)w ;
		sum += answer;
	}

	/* add back carry outs from top 16 bits to low 16 bits */
	sum = (sum >> 16) + (sum & 0xffff);     /* add hi 16 to low 16 */
	sum += (sum >> 16);                     /* add carry */
	answer = ~sum;                          /* truncate to 16 bits */
	return(answer);
}

void generatePacket(struct icmphdr *header, int* data, int id, int ttl)
{	
	memset(header, 0, sizeof(struct icmphdr));
	memset(data, 0, sizeof(int));
	header->type = ICMP_ECHO;
	header->code = 0;
	header->checksum = 0;
	header->un.echo.id = id;
	header->un.echo.sequence = ttl;
	
	*data = time(NULL);	
	
	header->checksum = in_cksum((unsigned short*)header, sizeof(struct icmphdr) + sizeof(int));
}

void init()
{
	nasz_socket = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (nasz_socket == -1)
	{
		perror("socket:");
		exit(1);
	}
}

void* senderThreadWorker(void *argument)
{
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
		generatePacket(header, data, 1, ttl);
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

void sigterm(int signo) 
{
}

void secondHandler(int signo)
{
}

void* sender(void *argument)
{
	//blokujemy SIGTERMa
    //~ sigemptyset(&inselect);
    //~ sigaddset(&inselect, SIGUSR1);
    //~ sigaddset(&outselect, SIGUSR2);
    //~ sigprocmask(SIG_BLOCK, &outselect, NULL);
    //~ sigprocmask(SIG_BLOCK, &inselect, NULL);
	
   /*
   for(int i = 0; i <= 3; i++)
   {
	   printf("cos wysylam\n");
	   usleep(500000);
   }
   */
   
   fd_set set;
   FD_ZERO(&set);
   FD_SET(0, &set);
   
   struct timespec asd;
   asd.tv_sec = 20;
   asd.tv_nsec = 0;
   
   sleep(30);
   for(int i =0;i<0xffffff; i++) 
   {
		;
   }
   errno = 0;
   printf("Przed pselect()\n");
   pselect(1, &set, NULL, NULL, &asd, &outselect);
   printf("Po pselect()\n");
   if (errno == EINTR)
        puts("Poszlo przerwanko.\n");
   else
        perror("pselect()");
   
   /*
   errno = 0;
   printf("Przed pselect()\n");
   pselect(1, &set, NULL, NULL, &asd, &oldset);
   printf("Po pselect()\n");
   if (errno == EINTR)
        puts("Poszlo przerwanko.\n");
   else
        perror("pselect()");
        * */
   
   return NULL;
}

void* receiver(void *argument)
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
	
	
    
   
	//~ //blokujemy SIGTERMa
    //~ sigemptyset(&inselect);
    //~ sigaddset(&inselect, SIGUSR1);
    //~ //sigaddset(&newset, SIGUSR2);
    //~ sigaddset(&outselect, SIGUSR2);
    //~ sigprocmask(SIG_BLOCK, &inselect, NULL);
	//~ 
	//~ float time = 2.5;
	//usleep(time*1000000);
	//sleep(5);
	//~ cout << "Wyslanie sygnalu 1" << endl;
//~ 
	//~ pthread_kill(sendingThread,SIGUSR1);
//~ 
	//~ cout << "Wyslanie sygnalu 2" << endl;
	//~ pthread_kill(sendingThread,SIGUSR2);
	//~ return NULL;
	return NULL;
}

int main()
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
    
    Module2 module2;
    std::string address = "212.77.98.9";
    int retries = 4;
	module2.init(address, retries);
	module2.startThreads();
	module2.joinThreads();
	  

    
    
    
    /*
    init();
    pthread_create(&senderThread, NULL, senderThreadWorker, NULL);
    pthread_create(&receivingThread, NULL, receiver, NULL);
    pthread_join(senderThread, NULL);
    pthread_join(receivingThread, NULL);
    */
    /*
	pthread_create(&sendingThread, NULL, sender, NULL);
	pthread_create(&receivingThread, NULL, receiver, NULL);
	
	sleep(10);
	printf("CZOKOCZAKI\n");
	
	pthread_join(sendingThread, NULL);
	pthread_join(receivingThread, NULL);
    printf("MAMO JUSZ\n");
    */
}
