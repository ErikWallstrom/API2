#include "player.h"
#include "../../log.h"

#define PLAYER_WIDTH 	10.0
#define PLAYER_HEIGHT 	16.0
#define PLAYER_SCALE 	4.0
#define PLAYER_SPEED 	200.0

const struct Vec2d player_standingfront[PLAYER_STANDINGCOUNT] = {
	{.x = 0.0, .y = 0.0}
};
const struct Vec2d player_standingback[PLAYER_STANDINGCOUNT] = {
	{.x = 10.0, .y = 0.0}
};
const struct Vec2d player_standingright[PLAYER_STANDINGCOUNT] = {
	{.x = 20.0, .y = 0.0}
};
const struct Vec2d player_standingleft[PLAYER_STANDINGCOUNT] = {
	{.x = 30.0, .y = 0.0}
};
const struct Vec2d player_walkingup[PLAYER_WALKINGVERTICALCOUNT] = {
	{.x = 10.0, .y = 16.0},
	{.x = 10.0, .y = 32.0}
};
const struct Vec2d player_walkingdown[PLAYER_WALKINGVERTICALCOUNT] = {
	{.x = 0.0, .y = 16.0},
	{.x = 0.0, .y = 32.0}
};
const struct Vec2d player_walkingright[PLAYER_WALKINGHORIZONTALCOUNT] = {
	{.x = 20.0, .y = 0.0},
	{.x = 20.0, .y = 16.0},
	{.x = 20.0, .y = 0.0},
	{.x = 20.0, .y = 32.0}
};
const struct Vec2d player_walkingleft[PLAYER_WALKINGHORIZONTALCOUNT] = {
	{.x = 30.0, .y = 0.0},
	{.x = 30.0, .y = 16.0},
	{.x = 30.0, .y = 0.0},
	{.x = 30.0, .y = 32.0}
};

static void player_updateanimation(struct GameLoop* loop, void* userdata)
{
	struct Player* self = userdata;
	self->frame++;
	size_t maxframe;
	if(self->animation == player_standingfront
		|| self->animation == player_standingback
		|| self->animation == player_standingleft
		|| self->animation == player_standingright)
	{
		maxframe = PLAYER_STANDINGCOUNT;
	}
	else if(self->animation == player_walkingup 
		|| self->animation == player_walkingdown)
	{
		maxframe = PLAYER_WALKINGVERTICALCOUNT;
	}
	else
	{
		maxframe = PLAYER_WALKINGHORIZONTALCOUNT;
	}

	if(self->frame >= maxframe)
	{
		self->frame = 0;
	}
}

struct Player* player_ctor(
	struct Player* self, 
	struct Vec2d pos, 
	struct GameLoop* loop)
{
	log_assert(self, "is NULL");
	log_assert(loop, "is NULL");

	rect_ctor(
		&self->rect, 
		pos, 
		RECTREGPOINT_CENTER, 
		PLAYER_WIDTH * PLAYER_SCALE,
		PLAYER_HEIGHT * PLAYER_SCALE
	);

	self->frame = 0;
	self->oldpos = self->rect.pos;
	self->direction = (struct Vec2d){0};
	self->animation = player_standingfront;
	self->loop = loop;
	gameloop_addcallback(
		loop, 
		(struct GameLoopCallback){player_updateanimation, self},
		200
	);

	return self;
}

void player_update(struct Player* self)
{
	log_assert(self, "is NULL");

	int stoppedx = 0;
	int stoppedy = 0;
	if(self->direction.y > 0.0)
	{
		self->animation = player_walkingdown;
	}
	else if(self->direction.y < 0.0)
	{
		self->animation = player_walkingup;
	}
	else
	{
		stoppedy = 1;
	}
	
	if(self->direction.x > 0.0)
	{
		if(fabs(self->direction.y) < 0.7) //Set to 0.71 to make change animation
		{
			self->animation = player_walkingright;
		}
	}
	else if(self->direction.x < 0.0)
	{
		if(fabs(self->direction.y) < 0.7) //Set to 0.71 to make change animation
		{
			self->animation = player_walkingleft;
		}
	}
	else
	{
		stoppedx = 1;
	}

	if(stoppedx && self->animation == player_walkingleft)
	{
		self->animation = player_standingleft;
		self->frame = 0;
	}
	else if(stoppedx && self->animation == player_walkingright)
	{
		self->animation = player_standingright;
		self->frame = 0;
	}
	else if(stoppedy && self->animation == player_walkingup)
	{
		self->animation = player_standingback;
		self->frame = 0;
	}
	else if(stoppedy && self->animation == player_walkingdown)
	{
		self->animation = player_standingfront;
		self->frame = 0;
	}

	self->oldpos = self->rect.pos;
	vec2d_scale(
		&self->direction, 
		PLAYER_SPEED * (self->loop->tickrate / 1000.0),
		&self->direction
	);

	self->rect.pos.x += self->direction.x;
	self->rect.pos.y += self->direction.y;

	self->direction.x = 0.0;
	self->direction.y = 0.0;
}

void player_render(
	struct Player* self, 
	struct Texture* spritesheet,
	SDL_Renderer* renderer)
{
	log_assert(self, "is NULL");
	
	struct Vec2d srect = self->animation[self->frame];
	SDL_RenderCopy(
		renderer,
		spritesheet->raw,
		&(SDL_Rect){srect.x, srect.y, PLAYER_WIDTH, PLAYER_HEIGHT},
		&(SDL_Rect){
			self->oldpos.x + (self->rect.pos.x - self->oldpos.x) 
				* self->loop->interpolation,
			self->oldpos.y + (self->rect.pos.y - self->oldpos.y)
				* self->loop->interpolation,
			self->rect.width,
			self->rect.height
		}
	);
}

void player_dtor(struct Player* self)
{
	log_assert(self, "is NULL");
}

