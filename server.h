#ifndef server_h_
#define server_h_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
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

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "task.h"

using namespace std;
using namespace rapidjson;

struct package {
	void* delegate;
	int connection;
};

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
		void writeJSON(int connection, int taskNr);
		void parsingJSONToDocument(Document& document);
		void parsingAddressesJSONToTask(Document& document, Task& task);
		void parsingTasksJSONToParsedData(Document& document, ParsedData& parsedData);
};

#endif
