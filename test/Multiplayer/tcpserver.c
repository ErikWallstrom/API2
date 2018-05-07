#include "../../tcpserver.h"
#include "../../log.h"
#include <stdlib.h>

static void onerror(void* userdata)
{
	(void)userdata;
	abort();
}

void ondisconnect(
	struct TCPServer* server, 
	struct TCPClient* client, 
	void* userdata)
{
	log_info("%s:%s disconnected!", client->ip, client->port);
}

int onconnect(struct TCPServer* server, struct TCPClient* client, void* userdata)
{
	log_info("%s:%s connected!", client->ip, client->port);
	return 1; //Accept
}

int main(void)
{
	log_seterrorhandler(onerror, NULL);

	struct TCPServer server;
	tcpserver_ctor(&server, 25565, 4, onconnect, ondisconnect, NULL);
	log_info("Listening [%i]", server.port);

	while(1)
	{
		tcpserver_update(&server);
		for(size_t i = 0; i < vec_getsize(server.clients); i++)
		{
			if(tcpclient_recv(&server.clients[i]))
			{
				log_info(
					"Received state: %i, %i, %i, %i", 
					server.clients[i].delivery.buffer[1],
					server.clients[i].delivery.buffer[2],
					server.clients[i].delivery.buffer[3],
					server.clients[i].delivery.buffer[4]
				);
			}
		}
	}

	tcpserver_dtor(&server);
}

