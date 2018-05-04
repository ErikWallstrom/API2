#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "tcpclient.h"
#include "vec.h"

struct TCPServer
{
	Vec(struct TCPClient) clients;
	size_t maxclients;
	int socket;
	uint16_t port;
};

struct TCPServer* tcpserver_ctor(
	struct TCPServer* self, 
	uint16_t port, 
	size_t maxclients
);
void tcpserver_update(struct TCPServer* self);
void tcpserver_dtor(struct TCPServer* self);

#endif
