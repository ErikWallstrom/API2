#include "inputhandler.h"
#include "texture.h"
#include "window.h"
#include "font.h"
#include "file.h"
#include "rect.h"
#include "log.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>

#define defer(func) __attribute__((cleanup(func)))

struct Tile
{
	struct Rect;
	struct Texture* texture;
};

struct Animation
{
	Vec(SDL_Rect) srects;
	double delay;
	size_t frame;
};

struct Entity
{
	struct Rect;
	struct Vector2d oldpos;
	Vec(struct Animation) animations;
	double oldtime;
	size_t animation;
};

struct Game
{
	struct Window* window;
	struct InputHandler* input;
	struct Texture* grasstexture;
	struct Texture* playertexture;
	struct Texture* treetexture;
	struct Entity* player;
	Vec(struct Tile) scene;
	int* done;
	int debug;

} game;

__attribute__((constructor))
static void initialize(void)
{
	srand(time(NULL));
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

__attribute__((destructor))
static void cleanup(void)
{
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

static void generatemap(void)
{
	for(int x = 0; x < game.window->width; x += game.grasstexture->width * 4)
	{
		for(int y = 0; 
			y < game.window->height; 
			y += game.grasstexture->height * 4)
		{
			vec_pushback(
				game.scene,
				(struct Tile){
					{
						.pos = {x, y},
						.width = game.grasstexture->width * 4,
						.height = game.grasstexture->height * 4,
						.rpoint = RECTREGPOINT_TOPLEFT
					},
					.texture = rand() % 10 ? 
						game.grasstexture : 
						game.treetexture
				}
			);
		}
	}
}

static void update(double tickrate)
{
	static double time = 0.0;
	time += tickrate;

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

	game.player->oldpos = rect_getpos(game.player);
	double speed = game.window->width / 4 * tickrate / 1000.0;
	if(up)
	{
		game.player->pos.y -= speed;
	}

	if(down)
	{
		game.player->pos.y += speed;
	}

	if(left)
	{
		game.player->pos.x -= speed;
	}

	if(right)
	{
		game.player->pos.x += speed;
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

	if(time - game.player->oldtime >= 
		game.player->animations[game.player->animation].delay)
	{
		game.player->oldtime = time;
		game.player->animations[game.player->animation].frame++;

		if(vec_getsize(game.player->animations[game.player->animation].srects) <=
			game.player->animations[game.player->animation].frame)
		{
			game.player->animations[game.player->animation].frame = 0;
		}
	}

	if(game.player->pos.y - game.player->height / 2.0 < 0.0)
	{
		game.player->pos.y = game.window->height - game.player->height / 2.0;
		game.player->oldpos = game.player->pos;
		vec_clear(game.scene);
		generatemap();
	}
	else if(game.player->pos.y + game.player->height / 2.0 > game.window->height)
	{
		game.player->pos.y = 0 + game.player->height / 2.0;
		game.player->oldpos = game.player->pos;
		vec_clear(game.scene);
		generatemap();
	}
	else if(game.player->pos.x - game.player->width / 2.0 < 0.0)
	{
		game.player->pos.x = game.window->width - game.player->height / 2.0;
		game.player->oldpos = game.player->pos;
		vec_clear(game.scene);
		generatemap();
	}
	else if(game.player->pos.x + game.player->width / 2.0 > game.window->width)
	{
		game.player->pos.x = 0 + game.player->width / 2.0;
		game.player->oldpos = game.player->pos;
		vec_clear(game.scene);
		generatemap();
	}

	int hittop = 0; 
	int hitbottom = 0; 
	int hitleft = 0; 
	int hitright = 0; 

	struct Vector2d pos = rect_getpos(game.player);
	double x = pos.x;
	double y = pos.y;

	for(size_t i = 0; i < vec_getsize(game.scene); i++)
	{
		if(game.scene[i].texture == game.treetexture)
		{
			struct Vector2d pos2 = rect_getpos(game.scene + i);
			if(rect_hittop(game.player, pos.y - game.player->oldpos.y, game.scene + i))
			{
				hittop = 1;
				y = pos2.y + game.scene[i].height;
			}
			if(rect_hitbottom(game.player, pos.y - game.player->oldpos.y, game.scene + i))
			{
				hitbottom = 1;
				y = pos2.y - game.player->height - 1;
			}
			if(rect_hitleft(game.player, pos.x - game.player->oldpos.x, game.scene + i))
			{
				hitleft = 1;
				x = pos2.x + game.scene[i].width;
			}
			if(rect_hitright(game.player, pos.x - game.player->oldpos.x, game.scene + i))
			{
				hitright = 1;
				x = pos2.x - game.player->width;
			}
		}
	}

	if(hittop || hitbottom || hitleft || hitright)
	{
		rect_setpos(game.player, (struct Vector2d){x, y});
	}
}

static void render(double tickrate, double adt)
{
	for(size_t i = 0; i < vec_getsize(game.scene); i++)
	{
		struct Vector2d pos = rect_getpos(game.scene + i);
		SDL_RenderCopyEx(
			game.window->renderer,
			game.scene[i].texture->raw,
			NULL,
			&(SDL_Rect){
				.x = pos.x, 
				.y = pos.y, 
				.w = game.scene[i].width,
				.h = game.scene[i].height
			},
			game.scene[i].texture == game.grasstexture ? 180 : 0,
			NULL,
			SDL_FLIP_NONE
		);

		if(game.scene[i].texture == game.treetexture && game.debug)
		{
			SDL_SetRenderDrawColor(game.window->renderer, 255, 0, 255, 255);
			SDL_RenderDrawRect(
				game.window->renderer, 
				&(SDL_Rect){
					.x = game.scene[i].pos.x, 
					.y = game.scene[i].pos.y, 
					.w = game.scene[i].width,
					.h = game.scene[i].height
				}
			);
		}
	}

	struct Vector2d pos = rect_getpos(game.player);
	struct Vector2d oldpos = game.player->oldpos;

	SDL_RenderCopy(
		game.window->renderer,
		game.playertexture->raw,
		&game.player->animations[game.player->animation].
			srects[game.player->animations[game.player->animation].frame],
		&(SDL_Rect){
			.x = oldpos.x + (pos.x - oldpos.x) * (adt / tickrate), 
			.y = oldpos.y + (pos.y - oldpos.y) * (adt / tickrate), 
			.w = game.player->width,
			.h = game.player->height
		}
	);

	if(game.debug)
	{
		SDL_RenderDrawRect(
			game.window->renderer,
			&(SDL_Rect){
				.x = pos.x,
				.y = pos.y,
				.w = game.player->width,
				.h = game.player->height
			}
		);
	}

	SDL_SetRenderDrawColor(game.window->renderer, 0, 0, 255, 255);
	window_render(game.window);
}

int main(void)
{
	defer(window_dtor) struct Window window;
	window_ctor(&window, "Game Window", 16 * 4 * 25, 16 * 4 * 15, WINDOW_VSYNC);

	///*
	SDL_SetWindowFullscreen(window.raw, SDL_WINDOW_FULLSCREEN_DESKTOP);
	window.width = 1600;
	window.height = 900;
	//*/

	struct InputHandler input;
	inputhandler_ctor(&input);

	defer(texture_dtor) struct Texture playertexture;
	texture_ctorimage(&playertexture, "./spritesheet.png", window.renderer);

	defer(texture_dtor) struct Texture grasstexture;
	texture_ctorimage(&grasstexture, "./grass.png", window.renderer);

	defer(texture_dtor) struct Texture treetexture;
	texture_ctorimage(&treetexture, "./tree.png", window.renderer);

	struct Entity player = {
		.pos = {window.width / 2.0, window.height / 2.0},
		.width = playertexture.width / 3.0 * 4.0 - 1.0,
		.height = playertexture.height * 4.0 - 1.0,
		.rpoint = RECTREGPOINT_CENTER,
		.animations = vec_ctor(struct Animation, 0)
	};

	struct Animation animation = {
		.srects = vec_ctor(SDL_Rect, 0),
		.delay = 200
	};

	vec_pushback(animation.srects, (SDL_Rect){0, 0, 16, playertexture.height});
	vec_pushback(animation.srects, (SDL_Rect){16, 0, 16, playertexture.height});
	vec_pushback(animation.srects, (SDL_Rect){0, 0, 16, playertexture.height});
	vec_pushback(animation.srects, (SDL_Rect){32, 0, 16, playertexture.height});
	vec_pushback(player.animations, animation);

	Vec(struct Tile) scene = vec_ctor(struct Tile, 0);

	double tickrate = 1000.0 / 30.0;
	Uint64 oldtime = SDL_GetPerformanceCounter();
	double adt = 0.0; //accumulated delta time
	int done = 0;

	game.playertexture = &playertexture;
	game.grasstexture = &grasstexture;
	game.treetexture = &treetexture;
	game.player = &player;
	game.window = &window;
	game.input = &input;
	game.done = &done;
	game.scene = scene;

	generatemap();

	while(!done)
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

	vec_dtor(player.animations);
	vec_dtor(animation.srects);
}

