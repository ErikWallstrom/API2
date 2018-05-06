#include "../../inputhandler.h"
#include "../../initialize.h"
#include "../../argparser.h"
#include "../../tcpclient.h"
#include "../../gameloop.h"
#include "../../btclient.h"
#include "../../window.h"
#include "../../log.h"

struct Game
{
	union {
		struct BTClient btclient;
		struct TCPClient tcpclient;
	};
	struct Window window;
	struct InputHandler input;
	int uppressed;
	int downpressed;
	int leftpressed;
	int rightpressed;
	int isbt;
};

static void onargerror(void* udata)
{
	argparser_printhelp(udata);
	abort();
}

static void onerror(void* udata)
{
	(void)udata;
	abort();
}

void update(struct GameLoop* loop, void* userdata)
{
	struct Game* game = userdata;
	for(size_t i = 0; i < 2; i++) //Update twice the game speed
	{
		if(game->isbt)
		{
			btclient_update(&game->btclient);
			if(btclient_recv(&game->btclient))
			{
				
			}
		}
		else
		{
			tcpclient_update(&game->tcpclient);
			if(tcpclient_recv(&game->tcpclient))
			{
				
			}
		}
	}

	inputhandler_update(&game->input);

	int oldup = game->uppressed;
	int olddown = game->downpressed;
	int oldleft = game->leftpressed;
	int oldright = game->rightpressed;

	for(size_t i = 0; i < vec_getsize(game->input.events); i++)
	{
		switch(game->input.events[i].type)
		{
		case SDL_QUIT:
			loop->done = 1;
			break;

		case SDL_KEYDOWN:
			switch(game->input.events[i].key.keysym.sym)
			{
			case SDLK_q:
				loop->done = 1;
				break;

			case SDLK_w:
				game->uppressed = 1;
				break;
			case SDLK_s:
				game->downpressed = 1;
				break;
			case SDLK_a:
				game->leftpressed = 1;
				break;
			case SDLK_d:
				game->rightpressed = 1;
				break;
			}
			break;

		case SDL_KEYUP:
			switch(game->input.events[i].key.keysym.sym)
			{
			case SDLK_w:
				game->uppressed = 0;
				break;
			case SDLK_s:
				game->downpressed = 0;
				break;
			case SDLK_a:
				game->leftpressed = 0;
				break;
			case SDLK_d:
				game->rightpressed = 0;
				break;
			}
			break;
		}
	}

	if(oldup != game->uppressed
		|| olddown != game->downpressed 
		|| oldleft != game->leftpressed
		|| oldright != game->rightpressed)
	{
		char buffer[4];
		buffer[0] = game->uppressed;
		buffer[1] = game->downpressed;
		buffer[2] = game->leftpressed;
		buffer[3] = game->rightpressed;

		log_info("Sending state!");
		if(game->isbt)
		{
			btclient_send(&game->btclient, buffer, 4);
		}
		else
		{
			tcpclient_send(&game->tcpclient, buffer, 4);
		}
	}
}

void render(struct GameLoop* loop, void* userdata)
{
	struct Game* game = userdata;
	SDL_SetRenderDrawColor(game->window.renderer, 0, 255, 255, 255);
	window_render(&game->window);
}

void onbtdisconnect(struct BTClient* client, void* userdata)
{
	(void)userdata;
	log_info("Disconnected");
	abort();
}

void ontcpdisconnect(struct TCPClient* client, void* userdata)
{
	(void)userdata;
	log_info("Disconnected");
	abort();
}

int main(int argc, char* argv[])
{
	struct Game game;
	game.uppressed = 0;
	game.downpressed = 0;
	game.leftpressed = 0;
	game.rightpressed = 0;

	int windowflags = WINDOW_DEFAULT;
	struct ArgParserLongOpt options[] = {
		{"disable-vsync", "Disable vsync. May decrease input lag", 0},
		{"windowed", "Disable fullscreen", 0},
		{"bt-addr", "Connect to host with bluetooth address", 1},
		{"bt-name", "Connect to host with bluetooth name", 1},
		{"ip", "The ip of the host", 1},
		{"port", "The port of the host", 1},
	};

	struct ArgParser argparser;
	log_seterrorhandler(onargerror, &argparser);
	argparser_ctor(
		&argparser, 
		argc, 
		argv, 
		options, 
		sizeof options / sizeof *options
	);

	if(!argparser.results[0].used)
	{
		windowflags |= WINDOW_VSYNC;
	}

	if(!argparser.results[1].used)
	{
		windowflags |= WINDOW_FULLSCREEN;
	}

	if(argparser.results[2].used && argparser.results[3].used)
	{
		log_error(
			"%s and %s cannot be used at the same time",
			options[2].opt, 
			options[3].opt
		);
	}
	else if(!argparser.results[2].used 
		&& !argparser.results[3].used
		&& !(argparser.results[4].used && argparser.results[5].used))
	{
		log_error(
			"Either %s, %s or %s and %s has to be specified", 
			options[2].opt, 
			options[3].opt,
			options[4].opt,
			options[5].opt
		);
	}

	log_seterrorhandler(onerror, NULL);
	if(argparser.results[2].used)
	{
		game.isbt = 1;
		btclient_ctor(
			&game.btclient, 
			argparser.results[2].arg, 
			0, 
			onbtdisconnect, 
			NULL
		);
	}
	else if(argparser.results[3].used)
	{
		game.isbt = 1;
		btclient_ctor(
			&game.btclient, 
			argparser.results[3].arg, 
			1, 
			onbtdisconnect, 
			NULL
		);
	}
	else if(argparser.results[4].used && argparser.results[5].used)
	{
		game.isbt = 0;
		tcpclient_ctor(
			&game.tcpclient,
			argparser.results[4].arg,
			argparser.results[5].arg,
			ontcpdisconnect,
			NULL
		);
	}
	else
	{
		log_assert(0, "should'n happen'");
	}

	argparser_dtor(&argparser);
	initialize();

	window_ctor(&game.window, "Game Window", 1200, 675, windowflags);
	inputhandler_ctor(&game.input);
	
	struct GameLoop loop;
	gameloop_ctor(
		&loop, 
		30,
		(struct GameLoopCallback){update, &game},
		(struct GameLoopCallback){render, &game}
	);

	gameloop_start(&loop);
	gameloop_dtor(&loop);
	inputhandler_dtor(&game.input);
	window_dtor(&game.window);
	cleanup();

	if(game.isbt)
	{
		btclient_dtor(&game.btclient);
	}
	else
	{
		tcpclient_dtor(&game.tcpclient);
	}
}

