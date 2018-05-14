#ifndef PLAYER_H
#define PLAYER_H

#include "../../gameloop.h"
#include "../../texture.h"
#include "../../rect.h"

#define PLAYER_STANDINGCOUNT 1
#define PLAYER_WALKINGVERTICALCOUNT 2
#define PLAYER_WALKINGHORIZONTALCOUNT 4

extern const struct Vec2d player_standingfront[PLAYER_STANDINGCOUNT];
extern const struct Vec2d player_standingback[PLAYER_STANDINGCOUNT];
extern const struct Vec2d player_standingright[PLAYER_STANDINGCOUNT];
extern const struct Vec2d player_standingleft[PLAYER_STANDINGCOUNT];
extern const struct Vec2d player_walkingup[PLAYER_WALKINGVERTICALCOUNT];
extern const struct Vec2d player_walkingdown[PLAYER_WALKINGVERTICALCOUNT];
extern const struct Vec2d player_walkingright[PLAYER_WALKINGHORIZONTALCOUNT];
extern const struct Vec2d player_walkingleft[PLAYER_WALKINGHORIZONTALCOUNT];

struct Player
{
	struct Rect rect;
	struct Vec2d oldpos;
	struct Vec2d direction;
	const struct Vec2d* animation;
	struct GameLoop* loop;
	size_t frame;
};

struct Player* player_ctor(
	struct Player* self, 
	struct Vec2d pos, 
	struct GameLoop* loop
);
void player_update(struct Player* self);
void player_render(
	struct Player* self, 
	struct Texture* spritesheet,
	SDL_Renderer* renderer
);
void player_dtor(struct Player* self);

#endif
