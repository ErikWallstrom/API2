#include "btclient.h"
#include "log.h"

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h> //NOTE: Include order important
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#define BTCLIENT_MAXNUMINFO 32 //NOTE: Arbitrarily chosen
#define BTCLIENT_SEARCHTIME 4 //Search for 1.28 * BTCLIENT_SEARCHTIME sec

struct BTClient* btclient_ctor(
	struct BTClient* self, 
	const char* addr, 
	int isname,
	BTClientDisconnect ondisconnect,
	void* userdata)
{
	log_assert(self, "is NULL");
	log_assert(addr, "is NULL");
	log_assert(strlen(addr) < BTCLIENT_ADDRLENGTH, "invalid size (%s)", addr);

	struct sockaddr_rc serveraddr;
	serveraddr.rc_family = AF_BLUETOOTH;
	serveraddr.rc_channel = 1; //NOTE: Should this be modifiable?

	int device = hci_get_route(NULL); //Get first available bt device id
	if(device == -1)
	{
		log_error("%s", strerror(errno));
	}

	int devicesocket = hci_open_dev(device);
	if(devicesocket == -1)
	{
		log_error("%s", strerror(errno));
	}
		
	if(isname)
	{
		inquiry_info* info = malloc(sizeof(inquiry_info) * BTCLIENT_MAXNUMINFO);
		if(!info)
		{
			log_error("Memory allocation failed (%s)", strerror(errno));
		}

		int numinfo = hci_inquiry( //Start searching
			device, 
			BTCLIENT_SEARCHTIME,
			BTCLIENT_MAXNUMINFO, 
			NULL, 
			&info, 
			IREQ_CACHE_FLUSH //Do not display previously found devices
		);
		if(numinfo == -1)
		{
			log_error("%s", strerror(errno));
		}

		int found = 0;
		for(int i = 0; i < numinfo; i++)
		{
			int result = hci_read_remote_name(
				devicesocket, 
				&info[i].bdaddr, 
				BTCLIENT_MAXNAMELENGTH, 
				self->name, 
				0 //Timeout in milliseconds
			);
			if(result == -1)
			{
				log_error("%s", strerror(errno));
			}

			if(!strcmp(self->name, addr))
			{
				serveraddr.rc_bdaddr = info[i].bdaddr;
				ba2str(&info[i].bdaddr, self->addr);
				found = 1;
				break;
			}
		}

		if(!found)
		{
			log_error("Unable to find host \"%s\"", addr);
		}

		free(info);
	}
	else
	{
		int result = str2ba(addr, &serveraddr.rc_bdaddr);
		if(result == -1)
		{
			log_error("%s", strerror(errno));
		}

		result = hci_read_remote_name(
			devicesocket, 
			&serveraddr.rc_bdaddr,
			BTCLIENT_MAXNAMELENGTH, 
			self->name,
			0
		);
		if(result == -1)
		{
			strcpy(self->name, "Unknown");
		}

		strcpy(self->addr, addr);
	}

	close(devicesocket);
	self->socket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	if(self->socket == -1)
	{
		log_error("%s", strerror(errno));
	}

	int result = connect(
		self->socket, 
		(struct sockaddr*)&serveraddr, 
		sizeof serveraddr
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

	self->sendqueue = vec_ctor(struct BTClientTransmission, 0);
	self->delivery.length = 0; //No incoming delivery 
	self->ondisconnect = ondisconnect;
	self->userdata = userdata;
	return self;
}

void btclient_update(struct BTClient* self)
{
	log_assert(self, "is NULL");

	if(!self->delivery.length)
	{
		int result = recv(
			self->socket, 
			self->delivery.buffer, 
			BTCLIENT_BUFSIZE,
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
			else
			{
				self->sendqueue[queuelen - 1].progress += result;
			}
		}
	}
}

void btclient_send(struct BTClient* self, const char* msg, uint8_t len)
{
	//TODO: Check if msg need copying, implement send queue
	log_assert(self, "is NULL");
	log_assert(msg, "is NULL");
	log_assert(len < 255, "invalid length");
	log_assert(len > 0, "invalid length");

	struct BTClientTransmission transmission;
	memcpy(transmission.buffer + 1, msg, len);
	transmission.buffer[0] = len + 1;
	transmission.length = len + 1;
	transmission.progress = 0;
	vec_push(self->sendqueue, transmission);
}

int btclient_recv(struct BTClient* self)
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

void btclient_dtor(struct BTClient* self)
{
	log_assert(self, "is NULL");

	vec_dtor(self->sendqueue);
	close(self->socket);
}

