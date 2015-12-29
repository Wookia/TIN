#ifndef task_h_
#define task_h_

#include <cstring>
#include <string>
#include <iostream>
#include <ctime>

using namespace std;

//Task of traceroute operation for the particular IPs
class Task {
	public:
		string* ip;
		int taskNumber;
		int size;
		
		Task();
		~Task();
		Task(const Task& task);
		
		void initTask(int size);
		int generateNumber();
	private:
		static int counter;
};

class Addresses {
	public:
		int taskNumber;
		string** addresses;
		int size;
};

class ParsedData {
	public:
		Addresses* addresses;
		int size;
};

#endif
