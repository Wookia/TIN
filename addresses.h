#ifndef addresses_h_
#define addresses_h_

#include <cstring>
#include <string>
#include <iostream>
#include <ctime>

using namespace std;

class Addresses {
	public:
		long long int taskNumber;
		string** addresses;
		int size;
		int* sizeOfTraceroute;
		
		Addresses();
		~Addresses();
		
		void initAddresses(int size);
		void fillTracerouteAddresses(int i, int numberOfAddreses, string traceroute[]);
};

#endif
