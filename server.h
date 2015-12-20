#ifndef server_h_
#define server_h_

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
#include <pthread.h>
#include <signal.h>
#include "errno.h"
#include <arpa/inet.h>
#include <iostream>

using namespace std;

// Module 1 Server
class Server {
	private:
		int socketServer;
		int portNumber;
		const char* IPAddress;
		char dataSent[1024];
		char dataReceived[1024];
		struct sockaddr_in server;
		struct sockaddr_in client;
	public:
		Server();
		
		void* childThreadFunction(int connection);
		void logger(int connection);
		void reading(int connection);
		void writing(int connection);
		void doTraceroute();
		void getData();
		void sendJSON(int connection, int taskNr);
		void parsingJSONToData();
};

#endif
