#include "tcpserver.h"
#include "gameloop.h"
#include "log.h"
#include <stdlib.h>

uint64_t received = 0;
uint64_t sent = 0;

void update(struct GameLoop* loop, void* userdata)
{
	struct TCPServer* server = userdata;
	tcpserver_update(server);

	for(size_t i = 0; i < vec_getsize(server->clients); i++)
	{
		char buffer[TCPCLIENT_BUFSIZE];
		strcpy(buffer, "Hi!");
		log_info(
			"Sending: \"%s\" to %s:%s", 
			buffer, 
			server->clients[i].ip,
			server->clients[i].port
		);
		tcpclient_send(&server->clients[i], buffer, strlen(buffer));
		sent++;

		if(tcpclient_recv(&server->clients[i]))
		{
			log_info(
				"Received: \"%.*s\" from %s:%s", 
				server->clients[i].delivery.length - 1,
				server->clients[i].delivery.buffer + 1,
				server->clients[i].ip,
				server->clients[i].port
			);
			received++;
		}
	}
}

void onerror(void* userdata)
{
	(void)userdata;
	log_info("Sent: %li, received: %li", sent, received);
	abort();
}

int main(void)
{
	log_seterrorhandler(onerror, NULL);

	struct TCPServer server;
	tcpserver_ctor(&server, 80, 4);

	struct GameLoop loop;
	gameloop_ctor(
		&loop, 
		30, 
		(struct GameLoopCallback){update, &server}, 
		(struct GameLoopCallback){NULL, NULL}
	);

	gameloop_start(&loop);
	gameloop_dtor(&loop);
	tcpserver_dtor(&server);
	log_info("FINISHED! Sent: %li, received: %li", sent, received);
}

