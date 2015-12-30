#include "parseddata.h"

ParsedData::ParsedData() {

}

ParsedData::~ParsedData() {
	//delete[] addresses;
}

void ParsedData::initParsedData(int size) {
	addresses = new Addresses[size];
	
	ParsedData::size = size;
	
	return;
}
