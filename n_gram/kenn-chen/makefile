CC 	= g++
CFLAGS 	= -std=c++11
LIBS	= -lboost_program_options

all: ngram clean

ngram: main.o ngram.o
	$(CC) main.o ngram.o -o ngram $(LIBS)
main.o: main.cpp ngram.h
	$(CC) $(CFLAGS) -c main.cpp $(LIBS)
ngram.o: ngram.cpp ngram.h
	$(CC) $(CFLAGS) -c ngram.cpp
clean:
	-rm *.o
