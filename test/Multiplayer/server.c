#include "../../tcpserver.h"
#include "../../gameloop.h"
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

	char buffer[1];
	buffer[0] = (uint8_t)vec_getsize(server->clients);
	tcpclient_send(client, buffer, sizeof buffer); 
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
			/*
				int16_t x = *(int16_t*)(server.clients[i].delivery.buffer + 1);
				int16_t y = *(int16_t*)(server.clients[i].delivery.buffer + 3);
				float movex = *(float*)(server.clients[i].delivery.buffer + 5);
				float movey = *(float*)(server.clients[i].delivery.buffer + 9);
				log_info(
					"Received (%i, %i) [%f, %f] from %s:%s", 
					x, 
					y, 
					(double)movex, 
					(double)movey, 
					server.clients[i].ip, 
					server.clients[i].port
				);
			*/

				for(size_t j = 0; j < vec_getsize(server.clients); j++)
				{
					if(i != j)
					{
						char buffer[1 + sizeof(int16_t) * 2
							+ sizeof(float) * 2];
						buffer[0] = (uint8_t)i;
						memcpy(
							buffer + 1, 
							server.clients[i].delivery.buffer + 1, 
							12
						);

						tcpclient_send(
							&server.clients[j], 
							buffer, 
							sizeof buffer
						);
					}
				}
			}
		}

		gameloop_sleep(1);
	}

	tcpserver_dtor(&server);
}

