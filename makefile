CC = g++
CFLAGS  = -g -Wall -std=c++0x -pthread

# typing 'make' will invoke the first target entry in the file 
# (in this case the default target entry)
# you can name this target entry anything, but "default" or "all"
# are the most commonly used names by convention
#
default: TIN

# To create the executable file count we need the object files
# countwords.o, counter.o, and scanner.o:
#
TIN:  PacketGenerator.o SynchronizedQueue.o Module2.o main.o
	$(CC) $(CFLAGS) -o TIN main.o Module2.o PacketGenerator.o SynchronizedQueue.o

# To create the object file countwords.o, we need the source
# files countwords.c, scanner.h, and counter.h:
#
main.o:  main.cpp Module2.h
	$(CC) $(CFLAGS) -c main.cpp

# To create the object file counter.o, we need the source files
# counter.c and counter.h:
#

Module2.o:  Packet.h Module2.cpp Module2.h
	$(CC) $(CFLAGS) -c Module2.cpp

SynchronizedQueue.o:  SynchronizedQueue.cpp SynchronizedQueue.h 
	$(CC) $(CFLAGS) -c SynchronizedQueue.cpp

PacketGenerator.o:  PacketGenerator.cpp PacketGenerator.h 
	$(CC) $(CFLAGS) -c PacketGenerator.cpp

clean: 
	$(RM) TIN *.o *~
