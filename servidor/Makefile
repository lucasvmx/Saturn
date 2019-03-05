CC=gcc
SOURCES=src/main.c src/interface.c src/servidor.c
OUTPUT=bin/server.exe
LIBS=-lws2_32 -liphlpapi -lsqlite3
OPTIONS=-mwindows
OPTIONS_DEBUG=
WARNINGS=-Wall -Wextra
CP=cp
CD=cd
RC=src/recurso.rc
RC_BIN=bin/recurso.res

.PHONY: run
.PHONY: debug	

all:
	$(shell windres -i $(RC) -o $(RC_BIN) -O coff)
	$(CC) $(SOURCES) -o $(OUTPUT) $(RC_BIN) `pkg-config --cflags --libs gtk+-3.0 gthread-2.0` $(LIBS) $(OPTIONS) $(WARNINGS)
	$(shell cp 'ui/interface.glade' 'bin/interface.glade' && cp 'ui/iconfinder_Saturn_89145.ico' 'bin/iconfinder_Saturn_89145.ico')
	$(shell cp 'ui/iconfinder_search-database_49618.ico' 'bin/iconfinder_search-database_49618.ico')
	
debug:
	$(shell windres -i $(RC) -o $(RC_BIN) -O coff)
	$(CC) $(SOURCES) -o $(OUTPUT) $(RC_BIN) -DDEBUG `pkg-config --cflags --libs gtk+-3.0 gthread-2.0` $(LIBS) $(OPTIONS_DEBUG) $(WARNINGS)
	$(shell cp 'ui/interface.glade' 'bin/interface.glade' && cp 'ui/iconfinder_Saturn_89145.ico' 'bin/iconfinder_Saturn_89145.ico')
	$(shell cp 'ui/iconfinder_search-database_49618.ico' 'bin/iconfinder_search-database_49618.ico')

run:
	$(MAKE) -C bin