#include "server.h"

struct package {
	void* delegate;
	int connection;
};

void* childThreadFunctionDel(void* pack) {
	struct package packa =*((struct package*) pack);
	return reinterpret_cast<Server*>(packa.delegate)->childThreadFunction(packa.connection);
}

void* Server::childThreadFunction(int connection) {
	cout << "Thread No: " << pthread_self() << endl;
	reading(connection);
	writing(connection);	
	close(connection);
	return NULL;
}

Server::Server() {
	socketServer = socket(AF_INET, SOCK_STREAM, 0);
	if (socketServer == -1) {
		perror("socket");
		exit(1);
	}
	
	portNumber = 8080;
	IPAddress = "127.0.0.1";
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(IPAddress);
	server.sin_port = htons(portNumber);
	
	if (bind(socketServer, (struct sockaddr*) &server, sizeof(server)) == -1) {
		perror("bind");
		exit(1);
	}
	
	if (listen(socketServer, 10) == -1) {
		perror("listen");
		exit(1);
	}
	
	int connection;
	pthread_t* childThread;
	int i=1;
	socklen_t sockaddrClient;
	while(1) {
		sockaddrClient = sizeof(client);
		connection = accept(socketServer, (struct sockaddr*) &client, &sockaddrClient);
		if (connection == -1) {
			perror("accept");
			exit(1);
		}
		if(i==1) {
			childThread=(pthread_t*)malloc(sizeof(pthread_t));
		}
		else { 
			childThread=(pthread_t*)realloc(childThread,i*sizeof(pthread_t));
		}
		struct package pack;
		pack.delegate=reinterpret_cast<void*>(this);
		pack.connection=connection;
		pthread_create(&childThread[i-1], NULL, childThreadFunctionDel, reinterpret_cast<void*>(&pack));
		i++;
	}
	for(int i=0; sizeof(pthread_t)*i<sizeof(childThread); i++) {
		pthread_join(childThread[i], NULL);
	} 
}

void Server::logger(int connection) {
	///TODO: send response to HTTP POST message
	
}

void Server::reading(int connection) {
	if (recv(connection, dataReceived, sizeof(dataReceived), 0) == -1) {
		perror("reading");
		exit(1);
	}
	cout << dataReceived << endl;
}

void Server::writing(int connection) {
	const char* data = "HTTP/1.1 200 OK\r\nServer: TIN/1.0\r\nContent-Lenght: 67\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html><html><body><h1>Aj aj, kapitanie</h1></body></html>";
	if (send(connection, data, strlen(data), 0) == -1) {
		perror("writing");
		exit(1);
	}
}

/*void Server::sendJSON(int connection, int taskNr) {
	char* json;
	sprintf(json,"{ task: %d }", taskNr);
	char* data;
	sprintf(data,"HTTP/1.1 200 OK\r\nServer: TIN/1.0\r\nContent-Lenght: %ld\r\nConnection: close\r\nContent-Type: application/json\r\n\r\n%s",strlen(json),json);
	if (send(connection, data, strlen(data), 0) == -1) {
		perror("sendJSON");
		exit(1);
	}
}

void Server::parsingJSONToData() {
	char* readJSON;
	int i=0;
	for( ; dataReceived[i]!='\0'; i++) {
		if(dataReceived[i]=='{') {
			break;
		}
	}
	for(int j=0; dataReceived[i]!='\0'; i++,j++) {
		readJSON[j]=dataReceived[i];
	}
	
}

void Server::doTraceroute() {
	
}

void Server::getData() {
	
}*/
