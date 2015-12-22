#include "task.h"

int Task::counter = 0;

Task::Task(int size) {
	ip = new string[size];
	taskNumber=generateNumber();
}

Task::~Task() {
	//cout << ip << "adres " << ip[0] << endl;
	//delete ip;
}

int Task::generateNumber() {
	///TODO: przesunac 2 bity w lewo i dodac licznik zadania
	//moga przyjsc dwa zadania w jednej sekundzie
	return ((time(nullptr)<<4));//+((counter<4)?(counter++):(counter=0)));
}
