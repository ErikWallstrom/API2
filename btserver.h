#ifndef BTSERVER_H
#define BTSERVER_H

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

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

			int status = bind(
				self->socket, 
				(struct sockaddr*)&addr, 
				sizeof addr
			);
			if(status == -1)
			{
				log_error("%s", strerror(errno));
			}

			status = fcntl(
				self->socket, 
				F_SETFL, 
				fcntl(self->socket, F_GETFL, 0) | O_NONBLOCK
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

#endif
