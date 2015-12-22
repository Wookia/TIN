#include "task.h"

int Task::counter = 0;
Task::Task(int size) {
	ip = new string[size];
	taskNumber=generateNumber();
}

int Task::generateNumber() {
	///TODO: przesunac 2 bity w lewo i dodac licznik zadania
	//moga przyjsc dwa zadania w jednej sekundzie
	return ((time(nullptr)<<2)+((counter<4)?(counter++):(counter=0)));
}
