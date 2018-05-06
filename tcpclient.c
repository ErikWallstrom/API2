#include "tcpclient.h"
#include "log.h"

#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>

struct TCPClient* tcpclient_ctor(
	struct TCPClient* self, 
	const char* addr, 
	const char* port,
	TCPClientDisconnect ondisconnect,
	void* userdata)
{
	log_assert(self, "is NULL");
	log_assert(addr, "is NULL");
	log_assert(strlen(port) < TCPCLIENT_PORTLENGTH, "invalid size (%s)", port);

	struct addrinfo hints = {0};
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	struct addrinfo* info;
	int result = getaddrinfo(addr, port, &hints, &info);
	if(result != 0)
	{
		log_error("%s", gai_strerror(result));
	}

	self->socket = socket(AF_INET, SOCK_STREAM, 0);
	if(self->socket == -1)
	{
		log_error("%s", strerror(errno));
	}

	struct addrinfo* node;
	for(node = info; node != NULL; node = node->ai_next)
	{
		result = connect(self->socket, node->ai_addr, node->ai_addrlen);
		if(result == -1)
		{
			continue;
		}

		break;
	}

	if(node == NULL)
	{
		log_error("%s", strerror(errno));
	}

	strcpy(self->port, port);
	if(!inet_ntop(
		AF_INET, 
		&((struct sockaddr_in*)node->ai_addr)->sin_addr,
		self->ip,
		TCPCLIENT_ADDRLENGTH))
	{
		log_error("%s", strerror(errno));
	}

	freeaddrinfo(info);

	result = setsockopt(
		self->socket, 
		IPPROTO_TCP, 
		TCP_NODELAY, 
		&(int){1},
		sizeof(int)
	);
	if(result == -1)
	{
		log_error("%s", strerror(errno));
	}

	result = fcntl(self->socket, F_SETFL, O_NONBLOCK);
	if(result == -1)
	{
		log_error("%s", strerror(errno));
	}

	self->sendqueue = vec_ctor(struct TCPClientTransmission, 0);
	self->delivery.length = 0; //No incoming delivery 
	self->ondisconnect = ondisconnect;
	self->userdata = userdata;
	return self;
}

//NOTE: Should be called before tcpclient_recv and tcpclient_send
void tcpclient_update(struct TCPClient* self)
{
	log_assert(self, "is NULL");

	if(!self->delivery.length)
	{
		int result = recv(
			self->socket, 
			self->delivery.buffer, 
			TCPCLIENT_BUFSIZE,
			MSG_PEEK
		);
		if(result == -1)
		{
			if(errno != EAGAIN && errno != EWOULDBLOCK)
			{
				if(self->ondisconnect)
				{
					self->ondisconnect(self, self->userdata);
					return;
				}
				else
				{
					log_error("%s", strerror(errno));
				}
			}
		}
		else if(result == 0)
		{
			if(self->ondisconnect)
			{
				self->ondisconnect(self, self->userdata);
				return;
			}
			else
			{
				log_error("Disconnected (%s)", strerror(errno));
			}
		}
		else
		{
			self->delivery.length = (uint8_t)self->delivery.buffer[0];
			self->delivery.progress = 0;
		}
	}

	if(self->delivery.length)
	{
		if(self->delivery.length == self->delivery.progress)
		{ 
			//Already finished receiving
			self->delivery.length = 0;
		}
		else
		{
			int result = recv(
				self->socket, 
				self->delivery.buffer + self->delivery.progress, 
				self->delivery.length - self->delivery.progress,
				0
			);
			if(result == -1)
			{
				if(errno != EAGAIN && errno != EWOULDBLOCK)
				{
					if(self->ondisconnect)
					{
						self->ondisconnect(self, self->userdata);
						return;
					}
					else
					{
						log_error("%s", strerror(errno));
					}
				}
			}
			else if(result == 0)
			{
				if(self->ondisconnect)
				{
					self->ondisconnect(self, self->userdata);
					return;
				}
				else
				{
					log_error("Disconnected (%s)", strerror(errno));
				}
			}
			else
			{
				self->delivery.progress += result;
			} 
		}
	}

	size_t queuelen = vec_getsize(self->sendqueue);
	if(queuelen)
	{
		if(self->sendqueue[queuelen - 1].length 
			== self->sendqueue[queuelen - 1].progress)
		{ 
			//Already finished sending
			vec_popback(self->sendqueue);
			queuelen = vec_getsize(self->sendqueue);
		}
		else
		{
			int result = send(
				self->socket, 
				self->sendqueue[queuelen - 1].buffer 
					+ self->sendqueue[queuelen - 1].progress,
				self->sendqueue[queuelen - 1].length
					- self->sendqueue[queuelen - 1].progress,
				0 //MSG_NOSIGNAL
			);
			if(result == -1)
			{
				if(self->ondisconnect)
				{
					self->ondisconnect(self, self->userdata);
					return;
				}
				else
				{
					log_error("%s", strerror(errno));
				}
			}
			else
			{
				self->sendqueue[queuelen - 1].progress += result;
			}
		}
	}
}

void tcpclient_send(struct TCPClient* self, const char* msg, uint8_t len)
{
	//TODO: Check if msg need copying, implement send queue

	log_assert(self, "is NULL");
	log_assert(msg, "is NULL");
	log_assert(len < 255, "invalid length");
	log_assert(len > 0, "invalid length");

	struct TCPClientTransmission transmission;
	memcpy(transmission.buffer + 1, msg, len);
	transmission.buffer[0] = len + 1;
	transmission.length = len + 1;
	transmission.progress = 0;
	vec_push(self->sendqueue, transmission);
}

int tcpclient_recv(struct TCPClient* self)
{
	log_assert(self, "is NULL");

	if(self->delivery.length)
	{
		if(self->delivery.length == self->delivery.progress)
		{
			return 1;
		}
	}

	return 0;
}

void tcpclient_dtor(struct TCPClient* self)
{
	log_assert(self, "is NULL");

	vec_dtor(self->sendqueue);
	close(self->socket);
}

