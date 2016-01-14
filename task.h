#ifndef task_h_
#define task_h_

#include <cstring>
#include <string>
#include <iostream>
#include <ctime>
#include <sys/time.h>

using namespace std;

//Task of traceroute operation for the particular IPs
class Task {
	public:
		string* ip;
		long long int taskNumber;
		int size;
		
		Task();
		~Task();
		
		void initTask(int size);
		long long int generateNumber();
		
	private:
		static int counter;
};

#endif
