#ifndef PLAYER_H
#define PLAYER_H

#include "../../animation.h"
#include "../../texture.h"
#include "../../rect.h"

enum PlayerAnimation
{
	PLAYERANIMATION_STANDINGFRONT,
	PLAYERANIMATION_STANDINGBACK,
	PLAYERANIMATION_STANDINGRIGHT,
	PLAYERANIMATION_STANDINGLEFT,
	PLAYERANIMATION_WALKINGDOWN,
	PLAYERANIMATION_WALKINGUP,
	PLAYERANIMATION_WALKINGRIGHT,
	PLAYERANIMATION_WALKINGLEFT,
	PLAYERANIMATION_COUNT
};

struct Player
{
	struct Animation animations[PLAYERANIMATION_COUNT];
	struct Texture spritesheet;
	struct Rect rect;
	struct Vec2d oldpos;
	size_t selectedanimation;
};

#endif
