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

#include "task.h"
#include "SynchronizedQueue.h"
#include "module3.h"
#include "Packet.h"

#define TABLE_SIZE 1000

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
		stringstream dataSent;
		struct sockaddr_in server;
		struct sockaddr_in client;
		pthread_t managerThread;
		pthread_t childThread[TABLE_SIZE];
		int processCount;
		Module3 dataReciver;
		SynchronizedQueue<Packet>* queueInto;

	public:
		Server(SynchronizedQueue<Packet>* queueToModule2);
		
		void closeServer();
		void startThreads();
		void* managerThreadFunction(void* argument); 
		void* childThreadFunction(int connection);
		void communicationCenter(int connection);
		string reading(int connection);
		void writing(int connection);
		string createResponseToAddressesJSON(long long int taskNr, int& HTTPcode);
		string createResponseToTasksJSON(list<Result>& results, int& HTTPcode);
		void writeJSON(int connection, string& json, int HTTPcode);
		void parsingJSONToDocument(Document& document, string dataReceived)throw(string);
		void parsingAddressesJSONToTask(Document& document, Task& task)throw(string);
		void parsingTasksJSONToParsedData(Document& document, list<long long int>& tasksList)throw(string);
};

#endif
