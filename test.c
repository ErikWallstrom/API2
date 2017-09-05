#include "inputhandler.h"
#include "dobject.h"
#include "texture.h"
#include "window.h"
#include "font.h"
#include "file.h"
#include "log.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

struct Animation
{
	Vec(SDL_Rect) srects;
	double delay;
	size_t current;
};

struct Entity
{
	struct DObject;
	Vec(struct Animation) animations;
	double oldtime;
};

struct Game
{
	struct Window window;
	struct InputHandler input;
	struct Texture grasstexture;
	struct Texture playertexture;
	struct Entity player;

	int done;

} game;

void initialize(void)
{
	if(SDL_Init(SDL_INIT_EVERYTHING))
	{
		log_error(SDL_GetError());
	}

	if(!IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG))
	{
		log_error(IMG_GetError());
	}

	if(TTF_Init())
	{
		log_error(TTF_GetError());
	}
}

void cleanup(void)
{
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void update(double tickrate)
{
	static int up = 0;
	static int down = 0;
	static int left = 0;
	static int right = 0;

	inputhandler_update(&game.input);
	for(size_t i = 0; i < vec_getsize(game.input.events); i++)
	{
		switch(game.input.events[i].type)
		{
		case SDL_MOUSEBUTTONDOWN:
			break;

		case SDL_KEYDOWN:
			switch(game.input.events[i].key.keysym.sym)
			{
			case SDLK_w:
				up = 1;
				break;
			case SDLK_s:
				down = 1;
				break;
			case SDLK_a:
				left = 1;
				break;
			case SDLK_d:
				right = 1;
				break;
			default: 
				break;
			}
			break;

		case SDL_WINDOWEVENT:
			if(game.input.events[i].window.event == 
				SDL_WINDOWEVENT_CLOSE)
			{
				game.done = 1;
			}
			break;

		case SDL_QUIT:
			game.done = 1;
		}
	}

	game.player.oldpos = game.player.pos;
	double speed = game.window.width / 2 * tickrate / 1000.0;
	if(up)
	{
		game.player.pos.y -= speed;
	}

	if(down)
	{
		game.player.pos.y += speed;
	}

	if(left)
	{
		game.player.pos.x -= speed;
	}

	if(right)
	{
		game.player.pos.x += speed;
	}

	if(!game.input.keystate[SDL_SCANCODE_W])
	{
		up = 0;
	}

	if(!game.input.keystate[SDL_SCANCODE_S])
	{
		down = 0;
	}

	if(!game.input.keystate[SDL_SCANCODE_A])
	{
		left = 0;
	}

	if(!game.input.keystate[SDL_SCANCODE_D])
	{
		right = 0;
	}
}

void render(double tickrate, double adt)
{
	for(int x = 0; x < game.window.width; x += game.grasstexture.width * 4)
	{
		for(int y = 0; y < game.window.height; y += game.grasstexture.height * 4)
		{
			SDL_RenderCopy(
				game.window.renderer,
				game.grasstexture.raw,
				NULL,
				&(SDL_Rect){
					.x = x, 
					.y = y, 
					.w = game.grasstexture.width * 4, 
					.h = game.grasstexture.height * 4
				}
			);
		}
	}

	struct Vector2d pos = dobject_getpos(&game.player);
	struct Vector2d oldpos = dobject_getoldpos(&game.player);

	SDL_RenderCopy(
		game.window.renderer,
		game.playertexture.raw,
		NULL,
		&(SDL_Rect){
			.x = oldpos.x + (pos.x - oldpos.x) * (adt / tickrate), 
			.y = oldpos.y + (pos.y - oldpos.y) * (adt / tickrate), 
			.w = game.grasstexture.width * 4, 
			.h = game.grasstexture.height * 4
		}
	);

	SDL_SetRenderDrawColor(game.window.renderer, 0, 0, 255, 255);
	window_render(&game.window);
}

void test(int* testp)
{
	log_info("%i, yey!", *testp);
}

int main(void)
{
	initialize();

	__attribute__((cleanup(test))) int t = 12;

	window_ctor(&game.window, "Game Window", 1200, 800, WINDOW_VSYNC);
	inputhandler_ctor(&game.input);

	texture_ctorimage(&game.playertexture, "./player.png", game.window.renderer);
	texture_ctorimage(&game.grasstexture, "./grass.png", game.window.renderer);

	game.player = (struct Entity){
		.pos = {game.window.width / 2.0, game.window.height / 2.0},
		.width = game.playertexture.width * 4,
		.height = game.playertexture.height * 4,
		.rpoint = DOBJECTREG_CENTER,
		.animations = vec_ctor(struct Animation, 0)
	};

	double tickrate = 1000.0 / 30.0;
	Uint64 oldtime = SDL_GetPerformanceCounter();
	double adt = 0.0; //accumulated delta time

	while(!game.done)
	{
		adt += (double)((SDL_GetPerformanceCounter() - oldtime) * 1000)
			/ SDL_GetPerformanceFrequency();
		oldtime = SDL_GetPerformanceCounter();

		while(adt >= tickrate) //Should this be if?
		{
			update(tickrate);
			adt -= tickrate;
		}

		render(tickrate, adt);

#ifndef NDEBUG
		static unsigned seconds = 0;
		if(seconds != game.window.seconds)
		{
			int length = snprintf(NULL, 0, "%s%i", "FPS: ", game.window.fps);
			char buffer[length + 1];
			snprintf(buffer, length + 1, "%s%i", "FPS: ", game.window.fps);

			//NOTE: SDL_SetWindowTitle is really slow
			SDL_SetWindowTitle(game.window.raw, buffer);
			seconds = game.window.seconds;
		}
#endif
	}

	texture_dtor(&game.grasstexture);
	texture_dtor(&game.playertexture);
	window_dtor(&game.window);

	cleanup();
}

