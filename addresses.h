#ifndef addresses_h_
#define addresses_h_

#include <cstring>
#include <string>
#include <iostream>
#include <ctime>

using namespace std;

class Addresses {
	public:
		int taskNumber;
		string** addresses;
		int size;
		
		Addresses(int size) {}
		~Addresses() {}
};

#endif
