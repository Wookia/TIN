#ifndef parseddata_h_
#define parseddata_h_

#include <cstring>
#include <string>
#include <iostream>
#include <ctime>

#include "addresses.h"

using namespace std;

class ParsedData {
	public:
		Addresses* addresses;
		int size;
		
		ParsedData();
		~ParsedData();
		void initParsedData(int size);
};

#endif
