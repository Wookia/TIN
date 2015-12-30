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
#include "parseddata.h"

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
		struct sockaddr_in server;
		struct sockaddr_in client;
		
	public:
		Server();
		
		void* childThreadFunction(int connection);
		void logger(int connection);
		string reading(int connection);
		void writing(int connection);
		void doTraceroute();
		void getData();
		string createResponseToAddressesJSON(int taskNr);
		string createResponseToTasksJSON(ParsedData& parsedData);
		void writeJSON(int connection, string& json);
		void parsingJSONToDocument(Document& document, string dataReceived);
		void parsingAddressesJSONToTask(Document& document, Task& task);
		void parsingTasksJSONToParsedData(Document& document, ParsedData& parsedData);
};

#endif
