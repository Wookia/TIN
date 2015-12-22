#include "client.h"

Client::Client() {
	socketClient = socket(AF_INET, SOCK_STREAM, 0);
	if (socketClient == -1) {
		perror("client socket");
		exit(1);
	} 
	
	serverHost = gethostbyname("127.0.0.1");
	
	if(serverHost == NULL) {
		cerr << "client ERROR no such host" << endl;
		exit(0);
	}
	
	portNumber = 8080;
	
	int yes = 1;
	if (setsockopt(socketClient, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1 ) {
		perror("setsockopt");
		exit(1);
	}
	
	server.sin_family = AF_INET;
	memcpy((char*) &server.sin_addr, (char*) serverHost->h_addr, serverHost->h_length);
	server.sin_port = htons(portNumber);
	if (connect(socketClient, (struct sockaddr*) &server, sizeof(server)) == -1) {
		perror("client connect");
		exit(1);
	}
	writing();
	reading();
}

void Client::reading() {
	if (recv(socketClient, output, 1024, 0) == -1) {
		perror("client recv");
		exit(1);
	}
	
	cout << output << endl;
}

void Client::writing() {
	gets(data);
	
	if (send(socketClient, data, 1024, 0) == -1) {
		perror("client send");
		exit(1);
	}
}
