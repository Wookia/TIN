CC = g++
CFLAGS = -g -Wall -std=c++0x

client: client.o mainClientM1.o
	$(CC) $(CFLAGS) client.o mainClientM1.o -o client
mainClientM1.o: client.h mainClientM1.cpp
	$(CC) $(CFLAGS) -c mainClientM1.cpp -o mainClientM1.o
client.o: client.h client.cpp
	$(CC) $(CFLAGS) -c client.cpp -o client.o

clean:
	$(RM) *.o *~
