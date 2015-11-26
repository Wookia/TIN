#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "pthread.h"
#include "signal.h"
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include "errno.h"
#include <iostream>

///TODO: most likely need to set sigprocmask in one place only!

using namespace std;

pthread_t sendingThread, receivingThread;
sigset_t outselect, inselect;

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
