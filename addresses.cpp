#include "addresses.h"

Addresses::Addresses() {
	addresses = NULL;
	size = 0;
}

Addresses::~Addresses() {
	delete[] addresses;
}
