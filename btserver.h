#ifndef BTSERVER_H
#define BTSERVER_H

#include "btclient.h"
#include "vec.h"

#define BTSERVER_MAXNAMELENGTH BTCLIENT_MAXNAMELENGTH

struct BTServer;
//Return 0 if connection should be refused
typedef int(*BTServerConnect)(
	struct BTServer* server, 
	struct BTClient* client, 
	void* userdata
);
typedef void(*BTServerDisconnect)(
	struct BTServer* server,
	struct BTClient* client,
	void* userdata
);

struct BTServer
{
	char name[BTSERVER_MAXNAMELENGTH];
	BTServerConnect onconnect;
	BTServerDisconnect ondisconnect;
	void* userdata;
	Vec(struct BTClient) clients;
	size_t maxclients;
	int socket;
	int devicesocket;
};

struct BTServer* btserver_ctor(
	struct BTServer* self, 
	size_t maxclients,
	BTServerConnect onconnect, 
	BTServerDisconnect ondisconnect, 
	void* userdata
);
void btserver_update(struct BTServer* self);
void btserver_dtor(struct BTServer* self);

#endif
