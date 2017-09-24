#include "../inputhandler.h"
#include "../transition.h"
#include "../animation.h"
#include "../texture.h"
#include "../window.h"
#include "../font.h"
#include "../file.h"
#include "../rect.h"
#include "../log.h"

#define defer(func) __attribute__((cleanup(func)))

struct Tile
{
	struct Rect;
	struct Rect hitbox;
	struct Texture* texture;
};

struct Entity
{
	struct Rect;
	struct Rect hitbox;
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
};

static struct Game game;
static struct Transition trans;
double delta = 0.0;
double r = 0.0;

static void generatemap(void)
{
	for(int x = 0; x < game.window->width; x += game.grasstexture->width * 4)
	{
		for(int y = 0; 
			y < game.window->height;
			y += game.grasstexture->height * 4)
		{
			struct Tile tile;
			rect_ctor(
				&tile,
				(struct Vector2d){x, y},
				RECTREGPOINT_TOPLEFT,
				game.grasstexture->width * 4,
				game.grasstexture->height * 4
			);

			rect_ctor(
				&tile.hitbox,
				(struct Vector2d){x, y + game.grasstexture->height * 2},
				RECTREGPOINT_TOPLEFT,
				game.grasstexture->width * 4,
				game.grasstexture->height * 2
			);

			tile.texture = rand() % 10 ?
				game.grasstexture :
				game.treetexture;
			vec_pushback(game.scene, tile);;
		}
	}
}

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

	game.player->oldpos = game.player->pos;
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
}

static void handlecollisions(void)
{
	game.player->hitbox.pos.x = game.player->pos.x
		+ game.playertexture->width / 3.0;
	game.player->hitbox.pos.y = game.player->pos.y;
	struct Vector2d pos = game.player->hitbox.pos;

	for(size_t i = 0; i < vec_getsize(game.scene); i++)
	{
		if(game.scene[i].texture == game.treetexture)
		{
			if(rect_hittop(
				&game.player->hitbox,
				pos.y - game.player->oldpos.y,
				&game.scene[i].hitbox)
			)
			{
				game.player->hitbox.pos = (struct Vector2d){
					.x = pos.x,
					.y = game.scene[i].hitbox.pos.y +
						 game.scene[i].hitbox.height
				};
			}
			else if(rect_hitbottom(
				&game.player->hitbox,
				pos.y - game.player->oldpos.y,
				&game.scene[i].hitbox)
			)
			{
				game.player->hitbox.pos = (struct Vector2d){
					.x = pos.x,
					.y = game.scene[i].hitbox.pos.y -
						 game.player->hitbox.height
				};
			}
			else if(rect_hitleft(
				&game.player->hitbox,
				pos.x - game.player->oldpos.x,
				&game.scene[i].hitbox)
			)
			{
				game.player->hitbox.pos = (struct Vector2d){
					.x = game.scene[i].hitbox.pos.x +
						 game.scene[i].hitbox.width,
					.y = pos.y
				};
			}
			else if(rect_hitright(
				&game.player->hitbox,
				pos.x - game.player->oldpos.x,
				&game.scene[i].hitbox)
			)
			{
				game.player->hitbox.pos = (struct Vector2d){
					.x = game.scene[i].hitbox.pos.x -
						 game.player->hitbox.width,
					.y = pos.y
				};
			}

			pos = game.player->hitbox.pos;
		}
	}

 	game.player->pos.x = game.player->hitbox.pos.x
		- game.playertexture->width / 3.0;
	game.player->pos.y = game.player->hitbox.pos.y;
}

static void update(double tickrate)
{
	handleinputs(tickrate);
	handlecollisions();

	animation_update(&game.player->animations[game.player->animation], tickrate);

	int newscene = 0;
	if(game.player->pos.y < 0.0)
	{
		game.player->pos.y = game.window->height - game.player->height;
		newscene = 1;
	}
	else if(game.player->pos.y + game.player->height > game.window->height)
	{
		game.player->pos.y = 0;
		newscene = 1;
	}
	else if(game.player->pos.x < 0.0)
	{
		game.player->pos.x = game.window->width - game.player->height;
		newscene = 1;
	}
	else if(game.player->pos.x + game.player->width > game.window->width)
	{
		game.player->pos.x = 0;
		newscene = 1;
	}

	if(newscene)
	{
		game.player->oldpos = game.player->pos;
		vec_clear(game.scene);
		generatemap();
	}
}

static void render(double tickrate, double adt)
{
	transition_update(&trans, delta);
	for(size_t i = 0; i < vec_getsize(game.scene); i++)
	{
		struct Vector2d pos = game.scene[i].pos;
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
			SDL_SetRenderDrawColor(game.window->renderer, 255, r, r, 255);
			SDL_RenderDrawRect(
				game.window->renderer, 
				&(SDL_Rect){
					.x = game.scene[i].hitbox.pos.x,
					.y = game.scene[i].hitbox.pos.y,
					.w = game.scene[i].hitbox.width,
					.h = game.scene[i].hitbox.height
				}
			);
		}
	}

	struct Vector2d pos = game.player->pos;
	struct Vector2d oldpos = game.player->oldpos;

	SDL_SetTextureColorMod(game.playertexture->raw, r, r, r);
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
		struct Vector2d boxpos = game.player->hitbox.pos;
		SDL_RenderDrawRect(
			game.window->renderer,
			&(SDL_Rect){
				.x = boxpos.x,
				.y = boxpos.y,
				.w = game.player->hitbox.width,
				.h = game.player->hitbox.height
			}
		);
	}

	SDL_SetRenderDrawColor(game.window->renderer, 255, 255, 255, 255);
	window_render(game.window);
}

int main(void)
{
	defer(window_dtor) struct Window window;
	window_ctor(&window, "Game Window", 16 * 4 * 20, 16 * 4 * 12, WINDOW_VSYNC);

	/*
	SDL_SetWindowFullscreen(window.raw, SDL_WINDOW_FULLSCREEN_DESKTOP);
	window.width = 1600;
	window.height = 900;
	*/

	struct InputHandler input;
	inputhandler_ctor(&input);

	defer(texture_dtor) struct Texture playertexture;
	texture_ctorimage(&playertexture, "./res/spritesheet.png", window.renderer);

	defer(texture_dtor) struct Texture grasstexture;
	texture_ctorimage(&grasstexture, "./res/grass.png", window.renderer);

	defer(texture_dtor) struct Texture treetexture;
	texture_ctorimage(&treetexture, "./res/tree.png", window.renderer);

	struct Entity player;
	player.animations = vec_ctor(struct Animation, 0);
	player.animation = 0;
	rect_ctor(
		&player,
		(struct Vector2d){window.width / 2.0, window.height / 2.0},
		RECTREGPOINT_CENTER,
		playertexture.width / 3.0 * 4.0,
		playertexture.height * 4.0
	);
	rect_ctor(
		&player.hitbox,
		(struct Vector2d){window.width / 2.0, window.height / 2.0},
		RECTREGPOINT_CENTER,
		playertexture.width / 3.0 * 2.0,
		playertexture.height * 4.0
	);

	struct Animation animation = {
		.delay = 200,
		.numframes = 4,
		.srects = (SDL_Rect[]) {
			{0,  0, 16, playertexture.height},
			{16, 0, 16, playertexture.height},
			{0,  0, 16, playertexture.height},
			{32, 0, 16, playertexture.height}
		}
	};

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

	transition_ctor(&trans, TRANSITIONTYPE_FADE, 0, 255, 1000, &r);

	generatemap();
	float x = 0.1;
	printf("%f\n", x);

	while(!done)
	{
		delta = (double)((SDL_GetPerformanceCounter() - oldtime) * 1000)
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

	vec_dtor(player.animations);
}

