#include "tcpclient.h"
#include "gameloop.h"
#include "log.h"
#include <stdlib.h>

uint64_t received = 0;
uint64_t sent = 0;

void finish(struct GameLoop* loop, void* userdata)
{
	loop->done = 1;
}

void update(struct GameLoop* loop, void* userdata)
{
	struct TCPClient* client = userdata;
	tcpclient_update(client);

	if(tcpclient_recv(client))
	{
		log_info(
			"Received: \"%.*s\" from %s:%s", 
			client->delivery.length - 1, 
			client->delivery.buffer + 1,
			client->ip,
			client->port
		);

		received++;
	}
	else
	{
		char buffer[TCPCLIENT_BUFSIZE];
		strcpy(buffer, "Hello!");
		log_info("Sending: \"%s\" to %s:%s", buffer, client->ip, client->port);
		tcpclient_send(client, buffer, strlen(buffer));
		sent++;
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
	
	struct TCPClient client;
	tcpclient_ctor(&client, "test.foridas.se", "80");

	struct GameLoop loop;
	gameloop_ctor(
		&loop, 
		1000.0 / 30.0, 
		(struct GameLoopCallback){update, &client}, 
		(struct GameLoopCallback){NULL, NULL}
	);

	gameloop_addcallback(
		&loop, 
		(struct GameLoopCallback){finish, NULL},
		20000
	);
	gameloop_start(&loop);
	gameloop_dtor(&loop);
	tcpclient_dtor(&client);
	log_info("FINISHED! Sent: %li, received: %li", sent, received);
}

