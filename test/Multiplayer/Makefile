CC = gcc
WARNINGS = -Wall -Wextra -Wshadow -Wstrict-prototypes -Wdouble-promotion \
		   -Wjump-misses-init -Wnull-dereference -Wrestrict -Wlogical-op \
		   -Wduplicated-branches -Wduplicated-cond
CLIENT_LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf -lbluetooth -lm 
BTSERVER_LIBS = -lbluetooth
DEBUG_FLAGS = -Og -g3
RELEASE_FLAGS = -march=native -mtune=native -O2 -DNDEBUG
CLIENT_FILES = client.c ../../window.c ../../argparser.c ../../inputhandler.c \
			   ../../gameloop.c ../../vec.c ../../log.c ../../ansicode.c      \
			   ../../initialize.c ../../btclient.c ../../tcpclient.c
BTSERVER_FILES = btserver.c ../../vec.c ../../log.c ../../ansicode.c \
			   ../../btserver.c ../../btclient.c
TCPSERVER_FILES = tcpserver.c ../../vec.c ../../log.c ../../ansicode.c \
			   ../../tcpserver.c ../../tcpclient.c

debug:
	$(CC) $(CLIENT_FILES) $(WARNINGS) $(DEBUG_FLAGS) -o client $(CLIENT_LIBS)
	$(CC) $(BTSERVER_FILES) $(WARNINGS) $(DEBUG_FLAGS) -o btserver \
		  $(BTSERVER_LIBS)
	$(CC) $(TCPSERVER_FILES) $(WARNINGS) $(DEBUG_FLAGS) -o tcpserver

release:
	$(CC) $(CLIENT_FILES) $(WARNINGS) $(RELEASE_FLAGS) -o client $(CLIENT_LIBS)
	$(CC) $(BTSERVER_FILES) $(WARNINGS) $(RELEASE_FLAGS) -o btserver \
	      $(BTSERVER_LIBS)
	$(CC) $(TCPSERVER_FILES) $(WARNINGS) $(RELEASE_FLAGS) -o tcpserver
