#include "../../btserver.h"
#include "../../log.h"
#include <stdlib.h>

void onerror(void* userdata)
{
	(void)userdata;
	abort();
}

int main(void)
{
	log_seterrorhandler(onerror, NULL);

	struct BTServer server;
	btserver_ctor(&server, 1);

	log_info("Listening [%s]!", server.name);
	while(1)
	{
		btserver_update(&server);
		for(size_t i = 0; i < vec_getsize(server.clients); i++)
		{
			if(btclient_recv(&server.clients[i]))
			{
				log_info(
					"Received '%.*s' from %s [%s]", 
					server.clients[i].delivery.length - 1,
					server.clients[i].delivery.buffer + 1,
					server.clients[i].addr,
					server.clients[i].name
				);
			}
		}
	}

	btserver_dtor(&server);
}

