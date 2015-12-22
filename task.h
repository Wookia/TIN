#ifndef task_h_
#define task_h_

#include <string>
#include <iostream>
#include <ctime>

using namespace std;

//Task of traceroute operation for the particular IPs
class Task {
	public:
		string* ip;
		int taskNumber;
		
		Task(int size);
		~Task();
		
		int generateNumber();
	private:
		static int counter;
};

/*
class TaskResult {
	public:
		int taskNumber;
		char** addresses[traceroute[]];
};

class Addresses {
	public:
		Task task;
		char** addresses[traceroute[]];
};

class ParsedData {
	public:
		char* traceroute[];
};
*/
#endif
