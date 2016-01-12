CC = g++
CFLAGS = -g -Wall -std=c++0x -pthread

server: server.o mainServerM1.o task.o module3.o
	$(CC) $(CFLAGS) server.o mainServerM1.o task.o module3.o -o server
mainServerM1.o: server.h mainServerM1.cpp
	$(CC) $(CFLAGS) -c mainServerM1.cpp -o mainServerM1.o
server.o: server.h server.cpp
	$(CC) $(CFLAGS) -c server.cpp -o server.o
task.o: task.h task.cpp
	$(CC) $(CFLAGS) -c task.cpp -o task.o
module3.o: module3.h module3.cpp
	$(CC) $(CFLAGS) -c module3.cpp -o module3.o
clean:
	$(RM) *.o *~
