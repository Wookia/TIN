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
long long int Task::generateNumber() {	
	struct timeval tp;
    gettimeofday(&tp, NULL);
    long long int mslong = (long long int) tp.tv_sec * 1000L + tp.tv_usec / 1000;
    
    return mslong;
}
