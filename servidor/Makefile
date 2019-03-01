CC=gcc
SOURCES=src/main.c src/interface.c src/servidor.c
OUTPUT=bin/server.exe
LIBS=-lws2_32 -liphlpapi
CP=cp
CD=cd

.PHONY: run
	
all:
	$(CC) $(SOURCES) -o $(OUTPUT) `pkg-config --cflags --libs gtk+-3.0 gthread-2.0` $(LIBS)
	$(shell cp 'ui/interface.glade' 'bin/interface.glade')
	
run:
	$(MAKE) -C bin