CC      = g++
CFLAGS  = -std=c++11
LDFLAGS = -static-libstdc++

all: pos clean

pos: main.o pos.o util.o
	$(CC) main.o pos.o util.o -o pos $(LDFLAGS)
main.o: main.cpp pos.h cmdline.h
	$(CC) $(CFLAGS) -c main.cpp
pos.o: pos.cpp pos.h util.h
	$(CC) $(CFLAGS) -c pos.cpp
util.o: util.cpp util.h
	$(CC) $(CFLAGS) -c util.cpp
clean:
	-rm *.o