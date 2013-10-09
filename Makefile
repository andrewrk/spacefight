CC=g++
CFLAGS=-std=c++11 $(shell sdl2-config --cflags)
LIBS=$(shell sdl2-config --libs)

spacefight: main.o
	$(CC) -o spacefight main.o $(LIBS)

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

