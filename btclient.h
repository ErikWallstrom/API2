#ifndef BTCLIENT_H
#define BTCLIENT_H

#include <inttypes.h>
#include <stddef.h>
#include "vec.h"

#define BTCLIENT_ADDRLENGTH 18
#define BTCLIENT_BUFSIZE 256
#define BTCLIENT_MAXNAMELENGTH 32 //NOTE: Arbitrary chosen

struct BTClient;
typedef void(*BTClientDisconnect)(
	struct BTClient* client, 
	void* userdata
);

struct BTClientTransmission
{
	char buffer[BTCLIENT_BUFSIZE];
	uint8_t length;
	uint8_t progress;
};

struct BTClient
{
	char name[BTCLIENT_MAXNAMELENGTH];
	struct BTClientTransmission delivery;
	char addr[BTCLIENT_ADDRLENGTH];
	BTClientDisconnect ondisconnect;
	void* userdata;
	Vec(struct BTClientTransmission) sendqueue;
	int socket;
};

struct BTClient* btclient_ctor(
	struct BTClient* self, 
	const char* addr,
	int isname,
	BTClientDisconnect ondisconnect,
	void* userdata
);
void btclient_update(struct BTClient* self);
void btclient_send(struct BTClient* self, const char* msg, uint8_t len);
int btclient_recv(struct BTClient* self);
void btclient_dtor(struct BTClient* self);

#endif
