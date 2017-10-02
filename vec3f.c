#include "vec3f.h"
#include "log.h"
#include <math.h>

struct Vec3f* vec3f_ctor(struct Vec3f* self, float x, float y, float z)
{
	log_assert(self, "is NULL");

	self->x = x;
	self->y = y;
	self->z = z;

	return self;
}

void vec3f_add(struct Vec3f* self, struct Vec3f* vec, struct Vec3f* dest)
{
	log_assert(self, "is NULL");
	log_assert(vec, "is NULL");
	log_assert(dest, "is NULL");

	dest->x = self->x + vec->x;
	dest->y = self->y + vec->y;
	dest->z = self->z + vec->z;
}

void vec3f_sub(struct Vec3f* self, struct Vec3f* vec, struct Vec3f* dest)
{
	log_assert(self, "is NULL");
	log_assert(vec, "is NULL");
	log_assert(dest, "is NULL");

	dest->x = self->x - vec->x;
	dest->y = self->y - vec->y;
	dest->z = self->z - vec->z;
}

void vec3f_scale(struct Vec3f* self, float scalar, struct Vec3f* dest)
{
	log_assert(self, "is NULL");
	log_assert(dest, "is NULL");

	dest->x = self->x * scalar;
	dest->y = self->y * scalar;
	dest->z = self->z * scalar;
}

void vec3f_negate(struct Vec3f* self, struct Vec3f* dest)
{
	log_assert(self, "is NULL");
	log_assert(dest, "is NULL");

	dest->x = -self->x;
	dest->y = -self->y;
	dest->z = -self->z;
}

void vec3f_length(struct Vec3f* self, float* dest)
{
	log_assert(self, "is NULL");
	log_assert(dest, "is NULL");

	*dest = sqrtf(self->x * self->x + self->y * self->y + self->z * self->z);
}

void vec3f_dot(struct Vec3f* self, struct Vec3f* vec, float* dest)
{
	log_assert(self, "is NULL");
	log_assert(vec, "is NULL");
	log_assert(dest, "is NULL");

	*dest = self->x * vec->x + self->y * vec->y + self->z * vec->z;
}

void vec3f_cross(struct Vec3f* self, struct Vec3f* vec, struct Vec3f* dest)
{
	log_assert(self, "is NULL");
	log_assert(vec, "is NULL");
	log_assert(dest, "is NULL");

	*dest = (struct Vec3f){
		self->y * vec->z - self->z * vec->y,
		self->z * vec->x - self->x * vec->z,
		self->x * vec->y - self->y * vec->x
	};
}

