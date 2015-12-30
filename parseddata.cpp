#include "parseddata.h"

ParsedData::ParsedData() {
	addresses = NULL;
	size = 0;
}

ParsedData::~ParsedData() {
	//delete[] addresses;
}

void ParsedData::initParsedData(int size) {
	addresses = new Addresses[size];
	
	ParsedData::size = size;
	
	return;
}
