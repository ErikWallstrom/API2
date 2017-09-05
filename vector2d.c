#include "vector2d.h"
#include "log.h"
#include <math.h>

struct Vector2d* vector2d_ctor(struct Vector2d* self, double x, double y)
{
	log_assert(self, "is NULL");
	self->x = x;
	self->y = y;

	return self;
}

void vector2d_add(
	struct Vector2d* self, 
	struct Vector2d* addend, //augend + addend = sum
	struct Vector2d* dest
)
{
	log_assert(self, "is NULL");
	log_assert(addend, "is NULL");
	log_assert(dest, "is NULL");

	dest->x = self->x + addend->x;
	dest->y = self->y + addend->y;
}

void vector2d_subtract(
	struct Vector2d* self, 
	struct Vector2d* subtrahend, //minuend - subtrahend = difference
	struct Vector2d* dest
)
{
	log_assert(self, "is NULL");
	log_assert(subtrahend, "is NULL");
	log_assert(dest, "is NULL");

	dest->x = self->x - subtrahend->x;
	dest->y = self->y - subtrahend->y;
}

void vector2d_scale(
	struct Vector2d* self, 
	double scalar,
	struct Vector2d* dest
)
{
	log_assert(self, "is NULL");
	log_assert(dest, "is NULL");

	dest->x = self->x * scalar;
	dest->y = self->y * scalar;
}

double vector2d_getlength(struct Vector2d* self)
{
	log_assert(self, "is NULL");
	return sqrt(self->x * self->x + self->y * self->y);
}

void vector2d_normalize(struct Vector2d* self, struct Vector2d* dest)
{
	log_assert(self, "is NULL");
	log_assert(dest, "is NULL");

	double length = vector2d_getlength(self);
	if(length == 0.0) //Prevent division by zero exceptions
	{
		dest->x = 0.0;
		dest->y = 0.0;
	}
	else
	{
		dest->x = self->x / length;
		dest->y = self->y / length;
	}
}

