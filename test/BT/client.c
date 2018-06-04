#include "../../btclient.h"
#include "../../gameloop.h"
#include "../../log.h"
#include <stdlib.h>

const char* messages[] = {
	"Hello\n",
	"World\n",
	"says Erik\n",
	"asldkjhaslkjdhaslkjdhaslkjdhaslkjdh\n",
	":DDDDDDDDDDDDDD\n",
	"        message is done ;) åäö\n",
	"...\n",
};
size_t currentmessage = 0;

void update(struct GameLoop* loop, void* userdata)
{
	struct BTClient* client = userdata;
	btclient_update(client);
}

void onsend(struct GameLoop* loop, void* userdata)
{
	struct BTClient* client = userdata;
	log_info("Sending \"%s\"...", messages[currentmessage]);
	btclient_send(
		client, 
		messages[currentmessage], 
		strlen(messages[currentmessage])
	);

	currentmessage++;
	if(currentmessage >= sizeof messages / sizeof *messages)
	{
		log_info("Done");
		loop->done = 1;
	}
}

void onerror(void* userdata)
{
	(void)userdata;
	abort();
}

void ondisconnect(struct BTClient* client, void* userdata)
{
	log_info("Disconnected");
}

int main(void)
{
	log_seterrorhandler(onerror, NULL);

	struct BTClient client;
	btclient_ctor(&client, "frida-laptop", 1, ondisconnect, NULL);
	
	log_info("Connected to %s [%s]!", client.addr, client.name);
	struct GameLoop loop;
	gameloop_ctor(
		&loop, 
		30, 
		(struct GameLoopCallback){update, &client},
		(struct GameLoopCallback){NULL, NULL}
	);

	gameloop_addcallback(
		&loop, 
		(struct GameLoopCallback){onsend, &client}, 
		2000
	);

	gameloop_start(&loop);
	gameloop_dtor(&loop);
	btclient_dtor(&client);
}

