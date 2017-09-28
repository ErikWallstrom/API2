#include "entities.h"
#include "../../log.h"

struct Entities* entities_ctor(struct Entities* self)
{
	log_assert(self, "is NULL");

	self->components = vec_ctor(int, 0);
	self->rects = vec_ctor(struct RectComponent, 0);
	self->moves = vec_ctor(struct MoveComponent, 0);
	self->animations = vec_ctor(struct AnimationComponent, 0);
	return self;
}

EntityID entites_add(struct Entities* self, int component)
{
	log_assert(self, "is NULL");

	size_t index = vec_getsize(self->components);
	vec_pushback(self->components, component);
	vec_expand(self->rects, index, 1);
	vec_expand(self->moves, index, 1);
	vec_expand(self->animations, index, 1);

	return index;
}

void entities_dtor(struct Entities* self)
{
	log_assert(self, "is NULL");

	vec_dtor(self->components);
	vec_dtor(self->animations);
	vec_dtor(self->moves);
	vec_dtor(self->rects);
}

