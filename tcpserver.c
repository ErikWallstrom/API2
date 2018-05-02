#include "tcpserver.h"
#include "log.h"
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

struct TCPServer* tcpserver_ctor(
	struct TCPServer* self, 
	uint16_t port, 
	size_t maxclients)
{
	log_assert(self, "is NULL");
	log_assert(maxclients, "is 0");

	self->port = port;
	self->maxclients = maxclients;
	self->clients = vec_ctor(struct TCPClient, maxclients);

	self->socket = socket(AF_INET, SOCK_STREAM, 0);
	if(self->socket == -1)
	{
		log_error("%s", strerror(errno));
	}

	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = htonl(INADDR_ANY),
		.sin_port = htons(port),
		.sin_zero = {0}
	};

	int status = bind(
		self->socket, 
		(struct sockaddr*)&addr, 
		sizeof addr
	);
	if(status == -1)
	{
		log_error("%s", strerror(errno));
	}

	status = listen(self->socket, maxclients);
	if(status == -1)
	{
		log_error("%s", strerror(errno));
	}

	status = fcntl(self->socket, F_SETFL, O_NONBLOCK);
	if(status == -1)
	{
		log_error("%s", strerror(errno));
	}

	return self;
}

void tcpserver_update(struct TCPServer* self)
{
	log_assert(self, "is NULL");

	if(vec_getsize(self->clients) < self->maxclients)
	{
		struct sockaddr_in addr;
		int client = accept(
			self->socket, 
			(struct sockaddr*)&addr, 
			&(socklen_t){sizeof addr}
		);

		if(client == -1)
		{
			if(errno != EWOULDBLOCK && errno != EAGAIN)
			{
				log_error("%s", strerror(errno));
			}
		}
		else
		{
			int result = setsockopt(
				client, 
				IPPROTO_TCP, 
				TCP_NODELAY, 
				&(int){1},
				sizeof(int)
			);
			if(result == -1)
			{
				log_error("%s", strerror(errno));
			}

			result = fcntl(client, F_SETFL, O_NONBLOCK);
			if(result == -1)
			{
				log_error("%s", strerror(errno));
			}

			struct TCPClient serverclient;
			serverclient.socket = client;
			serverclient.sendqueue = vec_ctor(
				struct TCPClientTransmission, 
				0
			);
			serverclient.delivery.length = 0; //No incoming delivery 
			sprintf(serverclient.port, "%i", ntohs(addr.sin_port));
			inet_ntop(
				AF_INET, 
				&addr.sin_addr.s_addr, 
				serverclient.ip, 
				sizeof serverclient.ip
			);
			vec_pushback(self->clients, serverclient);
			log_info(
				"User connected from %s:%s", 
				serverclient.ip, 
				serverclient.port
			);
		}
	}

	for(size_t i = 0; i < vec_getsize(self->clients); i++)
	{
		tcpclient_update(&self->clients[i]);
	}
}

void tcpserver_dtor(struct TCPServer* self)
{
	log_assert(self, "is NULL");

	for(size_t i = 0; i < vec_getsize(self->clients); i++)
	{
		tcpclient_dtor(&self->clients[i]);
	}

	vec_dtor(self->clients);
	close(self->socket);
}

