#include "../../inputhandler.h"
#include "../../transition.h"
#include "../../animation.h"
#include "../../texture.h"
#include "../../window.h"
#include "../../font.h"
#include "../../file.h"
#include "../../rect.h"
#include "../../log.h"
#include "entities.h"

#define defer(func) __attribute__((cleanup(func)))

struct Game
{
	struct Window* window;
	struct InputHandler* input;
	struct Entities* entities;
	int* done;
	int debug;
};

static struct Game game;

static void handleinputs(double tickrate)
{
	static int up = 0;
	static int down = 0;
	static int left = 0;
	static int right = 0;

	inputhandler_update(game.input);
	for(size_t i = 0; i < vec_getsize(game.input->events); i++)
	{
		switch(game.input->events[i].type)
		{
		case SDL_MOUSEBUTTONDOWN:
			break;

		case SDL_KEYDOWN:
			switch(game.input->events[i].key.keysym.sym)
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
			case SDLK_SPACE:
				game.debug = !game.debug;
				break;
			default: 
				break;
			}
			break;

		case SDL_WINDOWEVENT:
			if(game.input->events[i].window.event == 
				SDL_WINDOWEVENT_CLOSE)
			{
				*game.done = 1;
			}
			break;

		case SDL_QUIT:
			*game.done = 1;
		}
	}

	if(up)
	{
	}

	if(down)
	{
	}

	if(left)
	{
	}

	if(right)
	{
	}

	if(!game.input->keystate[SDL_SCANCODE_W])
	{
		up = 0;
	}

	if(!game.input->keystate[SDL_SCANCODE_S])
	{
		down = 0;
	}

	if(!game.input->keystate[SDL_SCANCODE_A])
	{
		left = 0;
	}

	if(!game.input->keystate[SDL_SCANCODE_D])
	{
		right = 0;
	}
}

static void handlecollisions(void)
{
}

static void update(double tickrate)
{
	handleinputs(tickrate);
	handlecollisions();
}

static void render(double tickrate, double adt)
{
	for(size_t i = 0; i < vec_getsize(game.entities->components); i++)
	{
		double renderx, rendery;
		if(game.entities->components[i] & COMPONENT_MOVABLE)
		{
			renderx = game.entities->moves[i].oldpos.x + 
				(game.entities->rects[i].pos.x - 
				 game.entities->moves[i].oldpos.x) * (adt / tickrate);
			rendery = game.entities->moves[i].oldpos.y +
				(game.entities->rects[i].pos.y - 
				 game.entities->moves[i].oldpos.y) * (adt / tickrate);
		}
		else
		{
			renderx = game.entities->rects[i].pos.x;
			rendery = game.entities->rects[i].pos.y;
		}

		SDL_Rect* srect;
		if(game.entities->components[i] & COMPONENT_ANIMATED)
		{
			struct Animation* animation = &game.entities->animations[i].
				animations[(int)game.entities->animations[i].selectedanimation];
			srect = &animation->srects[animation->frame];
		}
		else
		{
			srect = NULL;
		}

		SDL_RenderCopy(
			game.window->renderer, 
			game.entities->rects[i].texture->raw,
			srect,
			&(SDL_Rect){ 
				game.entities->rects[i].pos.x,
				game.entities->rects[i].pos.y,
				game.entities->rects[i].width,
				game.entities->rects[i].height,
			}
		);
	}

	SDL_SetRenderDrawColor(game.window->renderer, 255, 255, 255, 255);
	window_render(game.window);
}

int main(void)
{
	defer(window_dtor) struct Window window;
	window_ctor(
		&window, 
		"Game Window", 
		1200, 
		675, 
		WINDOW_VSYNC //| WINDOW_FULLSCREEN
	);

	defer(inputhandler_dtor) struct InputHandler input;
	inputhandler_ctor(&input);

	defer(texture_dtor) struct Texture playertexture;
	texture_ctorimage(&playertexture, "./res/spritesheet.png", window.renderer);

	defer(texture_dtor) struct Texture grasstexture;
	texture_ctorimage(&grasstexture, "./res/grass.png", window.renderer);

	defer(texture_dtor) struct Texture treetexture;
	texture_ctorimage(&treetexture, "./res/tree.png", window.renderer);

	defer(entities_dtor) struct Entities entities;
	entities_ctor(&entities);
	EntityID player = entites_add(
		&entities, 
		COMPONENT_RECTANGULAR | COMPONENT_MOVABLE
	);

	entities.rects[player] = (struct RectComponent){ 
		{
			.pos = {50, 50},
			.width = 100, 
			.height = 100
		},
		.texture = &playertexture
	};

	entities.moves[player] = (struct MoveComponent){
		.velocity = {5, 5},
		.oldpos = {100, 100}
	};

	double tickrate = 1000.0 / 30.0;
	Uint64 oldtime = SDL_GetPerformanceCounter();
	double adt = 0.0; //accumulated delta time
	int done = 0;

	game.entities = &entities;
	game.window = &window;
	game.input = &input;
	game.done = &done;

	while(!done)
	{
		double delta = (double)((SDL_GetPerformanceCounter() - oldtime) * 1000)
			/ SDL_GetPerformanceFrequency();

		adt += delta;
		oldtime = SDL_GetPerformanceCounter();

		while(adt >= tickrate) //Should this be if?
		{
			update(tickrate);
			adt -= tickrate;
		}

		render(tickrate, adt);

#ifndef NDEBUG
		static unsigned seconds = 0;
		if(seconds != game.window->seconds)
		{
			int length = snprintf(NULL, 0, "%s%i", "FPS: ", game.window->fps);
			char buffer[length + 1];
			snprintf(buffer, length + 1, "%s%i", "FPS: ", game.window->fps);

			//NOTE: SDL_SetWindowTitle is really slow
			SDL_SetWindowTitle(game.window->raw, buffer);
			seconds = game.window->seconds;
		}
#endif
	}
}

