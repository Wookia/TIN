#include "addresses.h"

Addresses::Addresses() {
	addresses = NULL;
	size = 0;
}

Addresses::~Addresses() {
	for (int i=0; i<size; i++) {
		delete[] addresses[i];
	}
	
	delete[] addresses;
	
	delete[] sizeOfTraceroute;
}

void Addresses::initAddresses(int size) {
	addresses = new string*[size];
	sizeOfTraceroute = new int[size];
	
	for (int i=0; i<size; i++) {
		addresses[i] = NULL;
	}
	
	Addresses::size = size;
	
	return;
}

void Addresses::fillTracerouteAddresses(int i, int numberOfAddresses, string traceroute[]) {
	addresses[i] = new string[numberOfAddresses];
	sizeOfTraceroute[i] = numberOfAddresses;
	
	for(int j=0; j<numberOfAddresses; j++) {
		addresses[i][j] = traceroute[j];
	}
	
	return;
}
