CC=gcc
SOURCES=src/main.c src/interface.c src/servidor.c
OUTPUT=bin/server32.exe
LIBS=-lws2_32 -liphlpapi -lsqlite3 -lxlsxwriter -lz
OPTIONS=-mwindows -D__32BIT__
OPTIONS_DEBUG=-DDEBUG
WARNINGS=-Wall -Wextra
CP=cp
CD=cd
RC=src/recurso.rc
RC_BIN=bin/recurso.res
ADICIONAL_INCLUDE_DIR=-I3rdparty/include
ADICIONAL_LIB_DIR=-L3rdparty/lib/libxlswriter/x86
PKG_CONFIG_FLAGS=`pkg-config --cflags --libs gtk+-3.0 gthread-2.0`

.PHONY: run
.PHONY: debug	

all:
	$(shell windres -i $(RC) -o $(RC_BIN) -O coff)
	$(CC) $(SOURCES) -o $(OUTPUT) $(RC_BIN) $(PKG_CONFIG_FLAGS) $(ADICIONAL_LIB_DIR) $(ADICIONAL_INCLUDE_DIR) $(LIBS) $(OPTIONS) $(WARNINGS)
	$(shell cp 'ui/interface.glade' 'bin/interface.xml' && cp 'ui/icone_saturno.ico' 'bin/icone_saturno.ico')
	$(shell cp 'ui/icone_database.ico' 'bin/icone_database.ico')
	
debug:
	$(shell windres -i $(RC) -o $(RC_BIN) -O coff)
	$(CC) $(SOURCES) -o $(OUTPUT) $(RC_BIN) $(PKG_CONFIG_FLAGS) $(ADICIONAL_LIB_DIR) $(ADICIONAL_INCLUDE_DIR) $(LIBS) $(OPTIONS_DEBUG) $(WARNINGS)
	$(shell cp 'ui/interface.glade' 'bin/interface.xml' && cp 'ui/icone_saturno.ico' 'bin/icone_saturno.ico')
	$(shell cp 'ui/icone_database.ico' 'bin/icone_database.ico')

run:
	$(MAKE) run32 -C bin
	
clean:
	rm -f bin/*.exe -vv
	rm -f bin/*.ico -vv
	rm -f bin/*.sdb -vv
	rm -f bin/*.xml -vv