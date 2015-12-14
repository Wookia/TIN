#include "server.h"

Server::Server() {
	socketServer = socket(AF_INET, SOCK_STREAM, 0);
	if (socketServer == -1) {
		perror("socket");
		exit(1);
	}
	portNumber = 80;
	IPAddress = "localhost";
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(2000);
	
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
	///TODO: send HTTP POST message
	
}

void Server::reading(int connection) {
	if (recv(connection,dataReceived, 100, 0) == -1) {
		perror("reading");
		exit(1);
	}
	cout << dataReceived << endl;
}

void Server::writing(int connection) {
	if (send(connection, "Aj aj, kapitanie", 100, 0) == -1) {
		perror("writing");
		exit(1);
	}
}
