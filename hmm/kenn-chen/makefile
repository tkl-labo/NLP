CC      = g++
CFLAGS  = -std=c++11 -O2
LDFLAGS = -static-libstdc++

all: pos clean

pos: main.o pos.o
	$(CC) main.o pos.o -o pos $(LDFLAGS)
main.o: main.cpp pos.h cmdline.h
	$(CC) $(CFLAGS) -c main.cpp
pos.o: pos.cpp pos.h
	$(CC) $(CFLAGS) -c pos.cpp
clean:
	-rm *.o
