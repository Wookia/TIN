#include "server.h"

Server::Server() {
	socketServer = socket(AF_INET, SOCK_STREAM, 0);
	if (socketServer == -1) {
		perror("socket");
		exit(1);
	}
	
	portNumber = 2000;
	IPAddress = "127.0.0.1";
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(IPAddress);
	server.sin_port = htons(portNumber);
	
	if (bind(socketServer, (struct sockaddr*) &server, sizeof(server)) == -1) {
		perror("bind");
		exit(1);
	}
	
	if (listen(socketServer, 5) == -1) {
		perror("listen");
		exit(1);
	}
	
	socklen_t sockaddrClient;
	int connection;
	while(1) {
		sockaddrClient = sizeof(client);
		connection = accept(socketServer, (struct sockaddr*) &client, &sockaddrClient);
		if (connection == -1) {
			perror("accept");
			exit(1);
		}
		
		int pid = fork();
		
		if (pid < 0) {
			perror("fork");
			exit(1);
		}
		else if (pid == 0) {
			close(socketServer);
			reading(connection);
			writing(connection);
			exit(0);
		}
		else {
			close(connection);
		}
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
	const char* data="HTTP/1.1 200 OK\r\nServer: nweb/22.0\r\nContent-Lenght: 67\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html><html><body><h1>Aj aj, kapitanie</h1></body></html>";
	if (send(connection, data, strlen(data), 0) == -1) {
		perror("writing");
		exit(1);
	}
}
