#include "server.h"

void* childThreadFunctionDel(void* pack) {
	struct package packa = *((struct package*) pack);
	return reinterpret_cast<Server*>(packa.delegate)->childThreadFunction(packa.connection);
}

Server::Server() {
	socketServer = socket(AF_INET, SOCK_STREAM, 0);
	if (socketServer == -1) {
		perror("socket");
		exit(1);
	}
	
	portNumber = 8080;
	IPAddress = "127.0.0.1";
    
    int yes = 1;
	if (setsockopt(socketServer, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}
		
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
	socklen_t sockaddrClient;
	
	int i = 1;
	while (1) {
		sockaddrClient = sizeof(client);
		connection = accept(socketServer, (struct sockaddr*) &client, &sockaddrClient);
		if (connection == -1) {
			perror("accept");
			exit(1);
		}
		if (i == 1) {
			childThread = (pthread_t*)malloc(sizeof(pthread_t));
		}
		else { 
			childThread = (pthread_t*)realloc(childThread,i*sizeof(pthread_t));
		}
		struct package pack;
		pack.delegate = reinterpret_cast<void*>(this);
		pack.connection = connection;
		pthread_create(&childThread[i-1], NULL, childThreadFunctionDel, reinterpret_cast<void*>(&pack));
		i++;
	}
	
	/* never gonna happen
	for(int i=0; sizeof(pthread_t)*i<sizeof(childThread); i++) {
		pthread_join(childThread[i], NULL);
	}
	*/ 
}

void* Server::childThreadFunction(int connection) {
	cout << "Thread No: " << pthread_self() << endl;
	
	logger(connection);
	close(connection);
	
	return NULL;
}

//converting JSON to appropiate object
void Server::parsingJSONToDocument(Document& document, string dataReceived) {
	//reading JSON part from HTTP message from Client
	string readJSON;
	
	//skipping through HTTP Header to JSON Object
	int i = 0;
	for ( ; dataReceived[i]!='{'; i++) ;
	for ( ; dataReceived[i]!='\0'; i++) {
		readJSON += dataReceived[i];
	}
	
	if (document.Parse(readJSON.c_str()).HasParseError()) {
		cerr << "parsing error" << endl;
		cout << readJSON << endl;
		exit(1);
	}
	
	return;
}

void Server::parsingAddressesJSONToTask(Document& document, Task& task) {
	//parsing first JSON
	assert(document.IsObject());
		
	assert(document["addresses"].IsArray());
	Value& addresses = document["addresses"];
	
	task.initTask(addresses.Size());
	
	cout << "parsingJSON, Task nr: " << task.taskNumber << endl;
	
	for (SizeType i = 0; i < addresses.Size(); i++) {
		assert(addresses[i]["address"].IsString());
		task.ip[i] = addresses[i]["address"].GetString();
	}
	
	return;
}

void Server::parsingTasksJSONToParsedData(Document& document, ParsedData& parsedData) {
	//parsing second JSON
    assert(document.IsObject());
    
	assert(document["tasks"].IsArray());
	Value& tasks = document["tasks"];
	
	parsedData.initParsedData(tasks.Size());
	
	for (SizeType i = 0; i < tasks.Size(); i++) {
		assert(tasks[i].HasMember("task"));
		assert(tasks[i]["task"].IsInt());
		parsedData.addresses[i].taskNumber = tasks[i]["task"].GetInt();
	}
	
	return;
}

// Logger reads HTTP message and writes the HTTP answer to client
void Server::logger(int connection) {
	//first read, then respond appriopiately
	string dataReceived=reading(connection);
	int HTTPcode;
	
	if (dataReceived[0] == 'P') { // POST
		Document document;
		parsingJSONToDocument(document, dataReceived);
		
		if (document.HasMember("addresses")) {
			Task task;
			parsingAddressesJSONToTask(document, task);
			
			cout << "Logger, Task nr: " << task.taskNumber << endl;
			
			for (int i=0; i<task.size; i++) {
				cout << "ip[" << i << "]: " <<task.ip[i] << endl;
			}
			
			string json = createResponseToAddressesJSON(task.taskNumber, HTTPcode);
			
			writeJSON(connection, json, HTTPcode);
			
			doTraceroute();
		}
		else if (document.HasMember("tasks")) {
			ParsedData parsedData;
			parsingTasksJSONToParsedData(document, parsedData);
			
			for(int i=0; i<parsedData.size; i++) {
				cout << "tasks[" << i << "]: " << parsedData.addresses[i].taskNumber << endl;
			}
						
			//sleep() or long loop
			sleep(4);
			
			getData();
			
			string json = createResponseToTasksJSON(parsedData, HTTPcode);
			
			writeJSON(connection, json, HTTPcode);
		}
	}
	else if (dataReceived[0] == 'G') { // GET
		writing(connection);
	}
	else {
		writing(connection);
	}
	
	return;
}

string Server::reading(int connection) {
	char dataReceived[10000];
	if (recv(connection, dataReceived, sizeof(dataReceived), 0) == -1) {
		perror("reading");
		exit(1);
	}
	cout << dataReceived << endl;
	string data = dataReceived;
	return data;
}

void Server::writing(int connection) {
	sprintf(dataSent,"HTTP/1.1 200 OK\r\nServer: TIN/1.0\r\nContent-Lenght: 67\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html><html><body><h1>Aj aj, kapitanie</h1></body></html>");
	if (send(connection, dataSent, strlen(dataSent), 0) == -1) {
		perror("writing");
		exit(1);
	}
	
	return;
}

string Server::createResponseToAddressesJSON(int taskNumber, int& HTTPcode) {
	//manually creating JSON
	string json;
	
	HTTPcode = 200;
	
	json += "{ \"task\": ";
	char taskNr[100];
	sprintf(taskNr, "%d", taskNumber);
	json += taskNr;
	json += " }";
	
	cout << "JSON Response to AddressesJSON: " << json << endl;
	
	return json;
}

string Server::createResponseToTasksJSON(ParsedData& parsedData, int& HTTPcode) {
	//manually creating JSON
	
	string json;
	
	///TODO: ResponseToTasksJSON
	
	if (parsedData.addresses[0].size == 0) {
		HTTPcode = 404;
		json = "";
		return json;
	}
	
	HTTPcode = 200;
	json += "{ \"tasks\": [ { } ] }";
	
	
	
	cout << "JSON Response to TasksJSON: " << json << endl;
	return json;
}

//writing HTTP Response with appropiate JSON
//writeJSON( connection, taskNr, object Task or other)
void Server::writeJSON(int connection, string& json, int HTTPcode) {
	if(json == "") {
		if (sprintf(dataSent,"HTTP/1.1 %d NOT FOUND\r\nServer: TIN/1.0\r\nConnection: close\r\n\r\n", HTTPcode) < 0 ) {
			cerr << "sprintf error";
			exit(1);
		}
	}
	else {
		if (sprintf(dataSent,"HTTP/1.1 %d OK\r\nServer: TIN/1.0\r\nContent-Lenght: %ld\r\nConnection: close\r\nContent-Type: application/json\r\n\r\n%s", HTTPcode, json.size(),json.c_str()) < 0 ) {
			cerr << "sprintf error";
			exit(1);
		}
	}
	
	if (send(connection, dataSent, strlen(dataSent), 0) == -1) {
		perror("sendJSON");
		exit(1);
	}
	
	return;
}

void Server::doTraceroute() {
	
	return;
}

void Server::getData() {
	
	return;
}
