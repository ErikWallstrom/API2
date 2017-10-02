#ifndef ENTITIES_H
#define ENTITIES_H

#include "../../rect.h"
#include "../../vec.h"

typedef size_t EntityID;

enum Component
{
	COMPONENT_RECTANGULAR = 1 << 0,
	COMPONENT_MOVABLE = 1 << 1,
	COMPONENT_ANIMATED = 1 << 2,
};

struct RectComponent
{
	struct Rect;
	struct Texture* texture;
};

struct MoveComponent
{
	struct Vec2d velocity;
	struct Vec2d oldpos;
};

struct AnimationComponent
{
	double selectedanimation;
	Vec(struct Animation) animations;
};

struct Entities
{
	Vec(int) components;
	Vec(struct RectComponent) rects;
	Vec(struct MoveComponent) moves;
	Vec(struct AnimationComponent) animations;
};

struct Entities* entities_ctor(struct Entities* self);
EntityID entites_add(struct Entities* self, int component);
void entities_dtor(struct Entities* self);

#endif
