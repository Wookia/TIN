#include "task.h"

int Task::counter = 0;

Task::Task() {
	ip = NULL;
	size = 0;
}

Task::~Task() {
	delete[] ip;
}

void Task::initTask(int size) {
	ip = new string[size];
	
	Task::size = size;
	
	taskNumber = generateNumber();
	
	return;
}
int Task::generateNumber() {
	///TODO: przesunac 2 bity w lewo i dodac licznik zadania
	//moga przyjsc dwa zadania w jednej sekundzie
	return ((time(nullptr)<<4));//+((counter<4)?(counter++):(counter=0)));
}
