#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "tcpclient.h"
#include "vec.h"

struct TCPServer;
//Return 0 if connection should be refused
typedef int(*TCPServerConnect)(
	struct TCPServer* server, 
	struct TCPClient* client, 
	void* userdata
);
typedef void(*TCPServerDisconnect)(
	struct TCPServer* server,
	struct TCPClient* client,
	void* userdata
);

struct TCPServer
{
	TCPServerConnect onconnect;
	TCPServerDisconnect ondisconnect;
	void* userdata;
	Vec(struct TCPClient) clients;
	size_t maxclients;
	int socket;
	uint16_t port;
};

struct TCPServer* tcpserver_ctor(
	struct TCPServer* self, 
	uint16_t port, 
	size_t maxclients,
	TCPServerConnect onconnect,
	TCPServerDisconnect ondisconnect,
	void* userdata
);
void tcpserver_update(struct TCPServer* self);
void tcpserver_dtor(struct TCPServer* self);

#endif
