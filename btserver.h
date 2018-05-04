#ifndef BTSERVER_H
#define BTSERVER_H

#include "btclient.h"
#include "vec.h"

#define BTSERVER_MAXNAMELENGTH BTCLIENT_MAXNAMELENGTH

struct BTServer
{
	char name[BTSERVER_MAXNAMELENGTH];
	Vec(struct BTClient) clients;
	size_t maxclients;
	int socket;
	int devicesocket;
};

struct BTServer* btserver_ctor(struct BTServer* self, size_t maxclients);
void btserver_update(struct BTServer* self);
void btserver_dtor(struct BTServer* self);

#endif
