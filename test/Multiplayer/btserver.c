#include "../../btserver.h"
#include "../../log.h"
#include <stdlib.h>

static void onerror(void* userdata)
{
	(void)userdata;
	abort();
}

int onconnect(struct BTServer* server, struct BTClient* client, void* userdata)
{
	log_info("%s [%s] connected!", client->addr, client->name);
	return 1; //Accept
}

int main(void)
{
	log_seterrorhandler(onerror, NULL);

	struct BTServer server;
	btserver_ctor(&server, 4, onconnect, NULL);
	log_info("Listening [%s]", server.name);

	while(1)
	{
		btserver_update(&server);
		for(size_t i = 0; i < vec_getsize(server.clients); i++)
		{
			if(btclient_recv(&server.clients[i]))
			{
				log_info(
					"Message from %s [%s]: %i, %i, %i, %i", 
					server.clients[i].addr,
					server.clients[i].name,
					server.clients[i].delivery.buffer[1],
					server.clients[i].delivery.buffer[2],
					server.clients[i].delivery.buffer[3],
					server.clients[i].delivery.buffer[4]
				);
			}
		}
	}

	btserver_dtor(&server);
}

