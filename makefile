CC = g++
CFLAGS = -g -Wall -std=c++0x -pthread

server: server.o mainServerM1.o task.o parseddata.o addresses.o
	$(CC) $(CFLAGS) server.o mainServerM1.o task.o parseddata.o addresses.o -o server
mainServerM1.o: server.h mainServerM1.cpp
	$(CC) $(CFLAGS) -c mainServerM1.cpp -o mainServerM1.o
server.o: server.h server.cpp
	$(CC) $(CFLAGS) -c server.cpp -o server.o
task.o: task.h task.cpp
	$(CC) $(CFLAGS) -c task.cpp -o task.o
parseddata.o: addresses.h parseddata.h parseddata.cpp
	$(CC) $(CFLAGS) -c parseddata.cpp -o parseddata.o
addresses.o: addresses.h addresses.cpp
	$(CC) $(CFLAGS) -c addresses.cpp -o addresses.o
clean:
	$(RM) *.o *~
