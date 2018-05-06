#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <inttypes.h>
#include <stddef.h>
#include "vec.h"

#define TCPCLIENT_ADDRLENGTH 16
#define TCPCLIENT_PORTLENGTH 6
#define TCPCLIENT_BUFSIZE 256

struct TCPClient;
typedef void(*TCPClientDisconnect)(
	struct TCPClient* client, 
	void* userdata
);

struct TCPClientTransmission
{
	char buffer[TCPCLIENT_BUFSIZE];
	uint8_t length;
	uint8_t progress;
};

struct TCPClient
{
	struct TCPClientTransmission delivery;
	char ip[TCPCLIENT_ADDRLENGTH];
	TCPClientDisconnect ondisconnect;
	void* userdata;
	Vec(struct TCPClientTransmission) sendqueue;
	char port[TCPCLIENT_PORTLENGTH];
	int socket;
};

struct TCPClient* tcpclient_ctor(
	struct TCPClient* self, 
	const char* ip, 
	const char* port,
	TCPClientDisconnect ondisconnect,
	void* userdata
);
void tcpclient_update(struct TCPClient* self);
void tcpclient_send(struct TCPClient* self, const char* msg, uint8_t len);
int tcpclient_recv(struct TCPClient* self);
void tcpclient_dtor(struct TCPClient* self);

#endif
