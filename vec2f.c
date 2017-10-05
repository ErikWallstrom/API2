#include "vec2f.h"
#include "log.h"
#include <math.h>

void vec2f_add(struct Vec2f* self, struct Vec2f* vec, struct Vec2f* dest)
{
	log_assert(self, "is NULL");
	log_assert(vec, "is NULL");
	log_assert(dest, "is NULL");

	dest->x = self->x + vec->x;
	dest->y = self->y + vec->y;
}

void vec2f_sub(struct Vec2f* self, struct Vec2f* vec, struct Vec2f* dest)
{
	log_assert(self, "is NULL");
	log_assert(vec, "is NULL");
	log_assert(dest, "is NULL");

	dest->x = self->x - vec->x;
	dest->y = self->y - vec->y;
}

void vec2f_scale(struct Vec2f* self, float scalar, struct Vec2f* dest)
{
	log_assert(self, "is NULL");
	log_assert(dest, "is NULL");

	dest->x = self->x * scalar;
	dest->y = self->y * scalar;
}

void vec2f_length(struct Vec2f* self, float* dest)
{
	log_assert(self, "is NULL");
	log_assert(dest, "is NULL");

	*dest = sqrtf(self->x * self->x + self->y * self->y);
}

void vec2f_normalize(struct Vec2f* self, struct Vec2f* dest)
{
	log_assert(self, "is NULL");
	log_assert(dest, "is NULL");

	float length;
	vec2f_length(self, &length);
	if(!length) //Prevent division by zero exceptions
	{
		dest->x = 0.0f;
		dest->y = 0.0f;
	}
	else
	{
		dest->x = self->x / length;
		dest->y = self->y / length;
	}
}

