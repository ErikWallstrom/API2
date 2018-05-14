#include "../../inputhandler.h"
#include "../../initialize.h"
#include "../../argparser.h"
#include "../../tcpclient.h"
#include "../../gameloop.h"
#include "../../window.h"
#include "../../log.h"
#include "player.h"

#define NUM_PLAYERS 4

struct Game
{
	struct Player players[NUM_PLAYERS];
	struct TCPClient client;
	struct Window window;
	struct Vec2d movements[NUM_PLAYERS];
	struct InputHandler input;
	struct Texture spritesheet;
	int hasmoved[NUM_PLAYERS];
	uint8_t localplayerid;
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
		tcpclient_update(&game->client);
		if(tcpclient_recv(&game->client))
		{
			uint8_t playerid = game->client.delivery.buffer[1];
			int16_t x = *(int16_t*)(game->client.delivery.buffer + 2);
			int16_t y = *(int16_t*)(game->client.delivery.buffer + 4);
			float movex = *(float*)(game->client.delivery.buffer + 6);
			float movey = *(float*)(game->client.delivery.buffer + 10);

			log_info(
				"Received |%i| (%i, %i) [%f, %f]", 
				playerid,
				x, 
				y, 
				(double)movex, 
				(double)movey
			);

			game->players[playerid].rect.pos.x = (double)x;
			game->players[playerid].rect.pos.y = (double)y;
			game->movements[playerid].x = (double)movex;
			game->movements[playerid].y = (double)movey;
			game->hasmoved[playerid] = 0;
		}
	}

	inputhandler_update(&game->input);
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
			}
			break;
		}
	}

	struct Vec2d movement = {0};
	if(game->input.keystate[SDL_SCANCODE_A]
		|| game->input.keystate[SDL_SCANCODE_H] 
		|| game->input.keystate[SDL_SCANCODE_LEFT] 
		|| SDL_GameControllerGetButton(
			game->input.controller,
			SDL_CONTROLLER_BUTTON_DPAD_LEFT))
	{
		movement.x += -1.0;
	}
	if(game->input.keystate[SDL_SCANCODE_D]
		|| game->input.keystate[SDL_SCANCODE_L] 
		|| game->input.keystate[SDL_SCANCODE_RIGHT] 
		|| SDL_GameControllerGetButton(
			game->input.controller,
			SDL_CONTROLLER_BUTTON_DPAD_RIGHT))
	{
		movement.x += 1.0;
	}
	if(game->input.keystate[SDL_SCANCODE_W]
		|| game->input.keystate[SDL_SCANCODE_K] 
		|| game->input.keystate[SDL_SCANCODE_UP] 
		|| SDL_GameControllerGetButton(
			game->input.controller,
			SDL_CONTROLLER_BUTTON_DPAD_UP))
	{
		movement.y += -1.0;
	}
	if(game->input.keystate[SDL_SCANCODE_S]
		|| game->input.keystate[SDL_SCANCODE_J] 
		|| game->input.keystate[SDL_SCANCODE_DOWN] 
		|| SDL_GameControllerGetButton(
			game->input.controller,
			SDL_CONTROLLER_BUTTON_DPAD_DOWN))
	{
		movement.y += 1.0;
	}

	int16_t xaxis = SDL_GameControllerGetAxis(
		game->input.controller, 
		SDL_CONTROLLER_AXIS_LEFTX
	);
	if(xaxis > 0)
	{
		movement.x += 1.0;
	}
	else if(xaxis < 0)
	{
		movement.x += -1.0;
	}

	int16_t yaxis = SDL_GameControllerGetAxis(
		game->input.controller, 
		SDL_CONTROLLER_AXIS_LEFTY
	);
	if(yaxis > 0)
	{
		movement.y += 1.0;
	}
	else if(yaxis < 0)
	{
		movement.y += -1.0;
	}

	vec2d_normalize(&movement, &movement);
	game->players[game->localplayerid].direction = movement;
	for(size_t i = 0; i < NUM_PLAYERS; i++)
	{
		if(i != game->localplayerid)
		{
			if(game->hasmoved[i])
			{
				game->players[i].direction = game->movements[i];
			}
			else
			{
				game->hasmoved[i] = 1;
			}
		}

		player_update(&game->players[i]);
	}

	if(game->movements[game->localplayerid].x != movement.x 
		|| game->movements[game->localplayerid].y != movement.y)
	{
		game->movements[game->localplayerid] = movement;

		char buffer[sizeof(int16_t) * 2 + sizeof(float) * 2];
		//NOTE: Not using network byte order
		*(int16_t*)(buffer + 0) = (int16_t)game->players[game->localplayerid]
			.rect.pos.x;
		*(int16_t*)(buffer + 2) = (int16_t)game->players[game->localplayerid]
			.rect.pos.y;
		*(float*)(buffer + 4) = (float)movement.x;
		*(float*)(buffer + 8) = (float)movement.y;

		log_info(
			"Sending (%i, %i) [%f, %f]",
			*(int16_t*)(buffer),
			*(int16_t*)(buffer + 2),
			(double)*(float*)(buffer + 4),
			(double)*(float*)(buffer + 8)
		);
		tcpclient_send(&game->client, buffer, sizeof buffer);
	}
}

void render(struct GameLoop* loop, void* userdata)
{
	struct Game* game = userdata;
	for(size_t i = 0; i < NUM_PLAYERS; i++)
	{
		player_render(
			&game->players[i], 
			&game->spritesheet, 
			game->window.renderer
		);
	}

	SDL_SetRenderDrawColor(game->window.renderer, 0, 255, 255, 255);
	window_render(&game->window);
}

void ontcpdisconnect(struct TCPClient* client, void* userdata)
{
	(void)userdata;
	log_info("Disconnected");
	abort();
}

int main(int argc, char* argv[])
{
	int windowflags = WINDOW_DEFAULT;
	struct ArgParserLongOpt options[] = {
		{"disable-vsync", "Disable vsync. May decrease input lag", 0},
		{"windowed", "Disable fullscreen", 0},
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

	if(!argparser.results[2].used && !argparser.results[3].used)
	{
		log_error(
			"%s and %s has to be specified", 
			options[2].opt, 
			options[3].opt
		);
	}

	log_seterrorhandler(onerror, NULL);
	struct Game game;
	memset(game.movements, 0, sizeof game.movements);
	memset(game.hasmoved, 0, sizeof game.hasmoved);
	tcpclient_ctor(
		&game.client,
		argparser.results[2].arg,
		argparser.results[3].arg,
		ontcpdisconnect,
		NULL
	);

	while(1)
	{
		tcpclient_update(&game.client);
		if(tcpclient_recv(&game.client))
		{
			game.localplayerid = (uint8_t)game.client.delivery.buffer[1];
			break;
		}

		gameloop_sleep(1);
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

	texture_ctorimage(
		&game.spritesheet, 
		"resources/sprites.png", 
		game.window.renderer
	);

	for(size_t i = 0; i < NUM_PLAYERS; i++)
	{
		player_ctor(
			&game.players[i], 
			(struct Vec2d){
				.x = game.window.width / 2.0, 
				.y = game.window.height / 2.0
			},
			&loop
		);
	}

	gameloop_start(&loop);
	gameloop_dtor(&loop);
	inputhandler_dtor(&game.input);
	window_dtor(&game.window);
	cleanup();

	tcpclient_dtor(&game.client);
}

