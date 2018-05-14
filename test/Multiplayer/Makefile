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
			   ../../initialize.c ../../tcpclient.c ../../texture.c 		  \
			   ../../rect.c ../../vec2d.c player.c
SERVER_FILES = server.c ../../vec.c ../../log.c ../../ansicode.c \
			   ../../tcpserver.c ../../tcpclient.c ../../gameloop.c

debug:
	$(CC) $(CLIENT_FILES) $(WARNINGS) $(DEBUG_FLAGS) -o client $(CLIENT_LIBS)
	$(CC) $(SERVER_FILES) $(WARNINGS) $(DEBUG_FLAGS) -o server

release:
	$(CC) $(CLIENT_FILES) $(WARNINGS) $(RELEASE_FLAGS) -o client $(CLIENT_LIBS)
	$(CC) $(SERVER_FILES) $(WARNINGS) $(RELEASE_FLAGS) -o server
