CC = g++-5
CFLAGS  = -g -Wall -std=c++0x -pthread
default: TIN

TIN:  PacketGenerator.o SynchronizedQueue.o task.o server.o module3.o Module2.o main.o
	$(CC) $(CFLAGS) main.o SynchronizedQueue.o server.o  Module2.o module3.o task.o PacketGenerator.o -o TIN 

main.o:  main.cpp SynchronizedQueue.h Module2.h server.h
	$(CC) $(CFLAGS) -c main.cpp

Module2.o:  Packet.h SynchronizedQueue.h Module2.cpp Module2.h module3.h 
	$(CC) $(CFLAGS) -c Module2.cpp

PacketGenerator.o:  PacketGenerator.cpp PacketGenerator.h
	$(CC) $(CFLAGS) -c PacketGenerator.cpp -o PacketGenerator.o
	
server.o: server.cpp server.h module3.h SynchronizedQueue.h task.h
	$(CC) $(CFLAGS) -c server.cpp
	
SynchronizedQueue.o:  SynchronizedQueue.cpp SynchronizedQueue.h
	$(CC) $(CFLAGS) -c SynchronizedQueue.cpp
	
task.o: task.h task.cpp
	$(CC) $(CFLAGS) -c task.cpp
	
module3.o: module3.h module3.cpp
	$(CC) $(CFLAGS) -c module3.cpp
	
clean:
	$(RM) TIN *.o *~
