#include "vec3f.h"
#include "log.h"
#include <math.h>

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

void vec3f_length(struct Vec3f* self, float* dest)
{
	log_assert(self, "is NULL");
	log_assert(dest, "is NULL");

	*dest = sqrtf(self->x * self->x + self->y * self->y + self->z * self->z);
}

void vec3f_normalize(struct Vec3f* self, struct Vec3f* dest)
{
	log_assert(self, "is NULL");
	log_assert(dest, "is NULL");

	float length;
	vec3f_length(self, &length);
	if(!length) //Prevent division by zero exceptions
	{
		dest->x = 0.0f;
		dest->y = 0.0f;
		dest->z = 0.0f;
	}
	else
	{
		dest->x = self->x / length;
		dest->y = self->y / length;
		dest->z = self->z / length;
	}
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

	dest->x = self->y * vec->z - self->z * vec->y;
	dest->y = self->z * vec->x - self->x * vec->z,
	dest->z = self->x * vec->y - self->y * vec->x;
}

