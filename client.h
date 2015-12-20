#ifndef client_h_
#define client_h_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include "errno.h"
#include <iostream>

using namespace std;

// Module 1 Client
class Client {
	private:
		int socketClient;
		int portNumber;
		char data[1024];
		char output[1024];
		struct sockaddr_in server;
		struct hostent* serverHost;
	public:
		Client();
		
		void reading();
		void writing();
};

#endif
