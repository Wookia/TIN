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


///TODO: most likely need to set sigprocmask in one place only!



using namespace std;

pthread_t sendingThread, receivingThread;
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
        while (nleft > 1)  {
                sum += *w++;
                nleft -= 2;
        }

        /* mop up an odd byte, if necessary */
        if (nleft == 1) {
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
	header->un.echo.id = (short)getpid();
	header->un.echo.sequence = id;
	
	*data = time(NULL);	
	
	header->checksum = in_cksum((unsigned short*)header, sizeof(struct icmphdr) + sizeof(int));
}

void sigterm(int signo) 
{
	printf("first");
}

void secondHandler(int signo)
{
	printf("second");
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
	//~ //blokujemy SIGTERMa
    //~ sigemptyset(&inselect);
    //~ sigaddset(&inselect, SIGUSR1);
    //~ //sigaddset(&newset, SIGUSR2);
    //~ sigaddset(&outselect, SIGUSR2);
    //~ sigprocmask(SIG_BLOCK, &inselect, NULL);
	//~ 
	float time = 2.5;
	//usleep(time*1000000);
	//sleep(5);
	cout << "Wyslanie sygnalu 1" << endl;
	//~ for(int i = 0; i<1000;i++)
	//~ {
	//~ pthread_kill(sendingThread,SIGUSR2);
	//~ }
	pthread_kill(sendingThread,SIGUSR1);
	//~ for(int i = 0; i<1000;i++)
	//~ {
	//~ pthread_kill(sendingThread,SIGUSR2);
	//~ }
	sleep(3);
	cout << "Wyslanie sygnalu 2" << endl;
	pthread_kill(sendingThread,SIGUSR2);
	return NULL;
}

int main()
{
	struct sockaddr_in addr;
	struct icmphdr* header = NULL;
	int ttl = 3;
	int* data = NULL;
	char buf[sizeof(struct icmphdr) + sizeof(int)];
	
	header = (struct icmphdr*)buf;
	data = (int*)(buf + sizeof(struct icmphdr));
	
	generatePacket(header, data, 1, 1);

	nasz_socket = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (nasz_socket == -1)
	{
		perror("socket:");
		exit(1);
	}
	setsockopt(nasz_socket, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
	
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	inet_aton("94.23.242.48", &addr.sin_addr);
	
	rc = sendto(nasz_socket,buf,sizeof(struct icmphdr) + sizeof(int),
				0, (struct sockaddr*)&addr, sizeof(addr));
	if(rc == -1)
	{
		perror("sendto:");
		exit(1);
	}
	
	
	
	
	
	
	
	
	
	
	//instalujemy handler
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
    
    //blokujemy SIGTERMa
    sigemptyset(&inselect);
    sigaddset(&inselect, SIGUSR1);
    //sigaddset(&newset, SIGUSR2);
    sigaddset(&outselect, SIGUSR2);
    sigprocmask(SIG_BLOCK, &inselect, NULL);
    
	printf("Tu bedzie sender:\n");
	pthread_create(&sendingThread, NULL, sender, NULL);
	printf("Tu bedzie receiver:\n");
	pthread_create(&receivingThread, NULL, receiver, NULL);
	
	sleep(10);
	printf("CZOKOCZAKI\n");
	
	pthread_join(sendingThread, NULL);
	pthread_join(receivingThread, NULL);
    printf("MAMO JUSZ\n");
}
