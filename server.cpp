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
void Server::parsingJSONToDocument(Document& document, string dataReceived)throw(string) {
	//reading JSON part from HTTP message from Client
	string readJSON;

	//skipping through HTTP Header to JSON Object
	int i = 0;
	for ( ; dataReceived[i]!='{'; i++) ;
	for ( ; dataReceived[i]!='\0'; i++) {
		readJSON += dataReceived[i];
	}

	if (document.Parse(readJSON.c_str()).HasParseError()) {
		throw(string("PARSING ERROR"));
	}

	return;
}

void Server::parsingAddressesJSONToTask(Document& document, Task& task)throw(string) {
    if(!document.IsObject())
        throw(string("BAD REQUEST"));

    if(!document["addresses"].IsArray())
        throw(string("BAD REQUEST"));
    Value& addresses = document["addresses"];

    task.initTask(addresses.Size());

    cout << "parsingJSON, Task nr: " << task.taskNumber << endl;

    for (SizeType i = 0; i < addresses.Size(); i++) {
        if(!addresses[i]["address"].IsString())
            throw(string("BAD REQUEST"));
        task.ip[i] = addresses[i]["address"].GetString();
    }

	return;
}

void Server::parsingTasksJSONToParsedData(Document& document, list<long long int>& tasksList)throw(string){
	//parsing second JSON
    if(!document.IsObject())
        throw(string("BAD REQUEST"));
    if(!document["tasks"].IsArray())
        throw(string("BAD REQUEST"));
    Value& tasks = document["tasks"];

    for (SizeType i = 0; i < tasks.Size(); i++) {
        if(!tasks[i].HasMember("task"))
            throw(string("BAD REQUEST"));
        if(!tasks[i]["task"].IsInt())
            throw(string("BAD REQUEST"));
        tasksList.push_back((long long int) tasks[i]["task"].GetInt());
    }
	return;
}

// Logger reads HTTP message and writes the HTTP answer to client
void Server::logger(int connection) {
	//first read, then respond appriopiately
	string dataReceived=reading(connection);
    string json;
	int HTTPcode;
    try
    {
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

                json = createResponseToAddressesJSON(task.taskNumber, HTTPcode);

                writeJSON(connection, json, HTTPcode);

                doTraceroute();
            }
            else if (document.HasMember("tasks")) {
                list<long long int> tasksList;
                parsingTasksJSONToParsedData(document, tasksList);
                list<Result> results;
                while(!tasksList.empty())
                {
                    results.push_back(dataReciver.getData(tasksList.front()));
                    tasksList.pop_front();
                }
                json = createResponseToTasksJSON(results, HTTPcode);

                writeJSON(connection, json, HTTPcode);
            }
        }
        else if (dataReceived[0] == 'G') { // GET
            writing(connection);
        }
        else {
            writing(connection);
        }
    }
    catch(string e)
    {
        json = "";
        writeJSON(connection, json, 400);
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

string Server::createResponseToAddressesJSON(long long int taskNumber, int& HTTPcode) {
	//manually creating JSON
	string json;

	HTTPcode = 200;

	json += "{ \"task\": ";
	char taskNr[100];
	sprintf(taskNr, "%Ld", taskNumber);
	json += taskNr;
	json += " }";

	cout << "JSON Response to AddressesJSON: " << json << endl;

	return json;
}

string Server::createResponseToTasksJSON(list<Result>& results, int& HTTPcode) {
	//manually creating JSON

	string json;

	///TODO: ResponseToTasksJSON

	if (results.empty()) {
		HTTPcode = 404;
		json = "";
	}
    else if (results.size() == 1 && results.front().taskNr == -1)
    {
        HTTPcode = 404;
		json = "";
    }
	else {
		HTTPcode = 200;

		json += "{ \"tasks\": [ ";
		while (!results.empty()) {
			json += "{ \"task\": ";
			char taskNr[100];
			Result result = results.front();
			results.pop_front();
			sprintf(taskNr, "%Ld", results.front().taskNr);
			json += taskNr;
			json += ", \"addresses\": [ ";
			while (!result.addresses.empty()) {
				Traceroute traceroute = result.addresses.front();
				result.addresses.pop_front();
				json += "{ \"traceroute\": [ ";
				while (!traceroute.road.empty()) {
					string address = traceroute.road.front();
					traceroute.road.pop_front();
					json += "{ \"address\": \"";
					json += address.c_str();
					json += "\"}";
					if (!traceroute.road.empty()) {
						json += ", ";
					}
				}
				json += " ] }";
				if (!result.addresses.empty()) {
					json += ", ";
				}
			}
			json += " ] }";
			if (!result.addresses.empty()) {
				json += ", ";
			}
		}
		json += "] }";

		cout << "JSON Response to TasksJSON: " << json << endl;
	}

	return json;
}

//writing HTTP Response with appropiate JSON
void Server::writeJSON(int connection, string& json, int HTTPcode) {
	if(HTTPcode == 404) {
		if (sprintf(dataSent,"HTTP/1.1 %d Nor Found\r\nServer: TIN/1.0\r\nConnection: close\r\n\r\n", HTTPcode) < 0 ) {
			cerr << "sprintf error";
			exit(1);
		}
	}
    else if(HTTPcode == 400) {
        if (sprintf(dataSent,"HTTP/1.1 %d Bad Request\r\nServer: TIN/1.0\r\nConnection: close\r\n\r\n", HTTPcode) < 0 ) {
			cerr << "sprintf error";
			exit(1);
		}
    }
	else {
		if (sprintf(dataSent,"HTTP/1.1 %d Ok\r\nServer: TIN/1.0\r\nContent-Lenght: %ld\r\nConnection: close\r\nContent-Type: application/json\r\n\r\n%s", HTTPcode, json.size(),json.c_str()) < 0 ) {
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
