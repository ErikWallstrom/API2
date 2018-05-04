#include "btserver.h"
#include "log.h"

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h> //NOTE: Include order important
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

struct BTServer* btserver_ctor(struct BTServer* self, size_t maxclients)
{
	log_assert(self, "is NULL");
	log_assert(maxclients, "is 0");

	self->maxclients = maxclients;
	self->clients = vec_ctor(struct BTClient, maxclients);

	self->socket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	if(self->socket == -1)
	{
		log_error("%s", strerror(errno));
	}

	struct sockaddr_rc addr = {
		.rc_family = AF_BLUETOOTH,
		.rc_bdaddr = *BDADDR_ANY,
		.rc_channel = 1
	};

	int result = bind(self->socket, (struct sockaddr*)&addr, sizeof addr);
	if(result == -1)
	{
		log_error("%s", strerror(errno));
	}

	result = listen(self->socket, maxclients);
	if(result == -1)
	{
		log_error("%s", strerror(errno));
	}

	result = fcntl(self->socket, F_SETFL, O_NONBLOCK);
	if(result == -1)
	{
		log_error("%s", strerror(errno));
	}

	int device = hci_get_route(NULL); //Get first available bt device id
	if(device == -1)
	{
		log_error("%s", strerror(errno));
	}

	self->devicesocket = hci_open_dev(device);
	if(self->devicesocket == -1)
	{
		log_error("%s", strerror(errno));
	}

	result = hci_read_local_name(
		self->devicesocket, 
		BTSERVER_MAXNAMELENGTH, 
		self->name,
		0
	);
	if(result == -1)
	{
		strcpy(self->name, "Unknown");
	}
	
	return self;
}

void btserver_update(struct BTServer* self)
{
	log_assert(self, "is NULL");

	if(vec_getsize(self->clients) < self->maxclients)
	{
		struct sockaddr_rc addr;
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
			int result = fcntl(client, F_SETFL, O_NONBLOCK);
			if(result == -1)
			{
				log_error("%s", strerror(errno));
			}

			struct BTClient serverclient;
			serverclient.socket = client;
			serverclient.sendqueue = vec_ctor(struct BTClientTransmission, 0);
			serverclient.delivery.length = 0; //No incoming delivery 
			ba2str(&addr.rc_bdaddr, serverclient.addr);
			result = hci_read_remote_name(
				self->devicesocket,
				&addr.rc_bdaddr,
				BTSERVER_MAXNAMELENGTH, 
				serverclient.name,
				0
			);
			if(result == -1)
			{
				strcpy(serverclient.name, "Unknown");
			}

			vec_pushback(self->clients, serverclient);
			log_info(
				"User connected from %s [%s]", 
				serverclient.addr,
				serverclient.name
			);
		}
	}

	for(size_t i = 0; i < vec_getsize(self->clients); i++)
	{
		btclient_update(&self->clients[i]);
	}
}

void btserver_dtor(struct BTServer* self)
{
	log_assert(self, "is NULL");

	for(size_t i = 0; i < vec_getsize(self->clients); i++)
	{
		btclient_dtor(&self->clients[i]);
	}

	vec_dtor(self->clients);
	close(self->socket);
	close(self->devicesocket);
}

