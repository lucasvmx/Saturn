CC=gcc
SOURCES=src/main.c src/interface.c
OUTPUT=bin/server.exe
LIBS=-lws2_32

all:
	$(CC) $(SOURCES) -o $(OUTPUT) `pkg-config --cflags --libs gtk+-3.0` $(LIBS)