CC = g++
CFLAGS = -g -Wall -std=c++0x -pthread

server: server.o mainServerM1.o task.o
	$(CC) $(CFLAGS) server.o mainServerM1.o task.o -o server
mainServerM1.o: server.h mainServerM1.cpp
	$(CC) $(CFLAGS) -c mainServerM1.cpp -o mainServerM1.o
server.o: server.h task.h server.cpp
	$(CC) $(CFLAGS) -c server.cpp -o server.o
task.o: task.h task.cpp
	$(CC) $(CFLAGS) -c task.cpp -o task.o
clean:
	$(RM) *.o *~
