#include "mat4f.h"
#include "log.h"

#include <math.h>

const struct Mat4f MAT4F_IDENTITYMATRIX = {
	{
		{1.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f},
	}
};

struct Mat4f* mat4f_ctor(struct Mat4f* self, float val)
{
	log_assert(self, "is NULL");

	for(size_t i = 0; i < 4; i++)
	{
		for(size_t j = 0; j < 4; j++)
		{
			self->scalars[i][j] = val;
		}
	}

	return self;
}

void mat4f_mulvec3f(struct Mat4f* self, struct Vec3f* vec, struct Vec3f* dest)
{
	log_assert(self, "is NULL");
	log_assert(vec, "is NULL");
	log_assert(dest, "is NULL");

	struct Vec3f v = *vec;
	dest->x = 
		  self->scalars[0][0] * v.x + self->scalars[1][0] * v.y 
		+ self->scalars[2][0] * v.z + self->scalars[3][0] * 1.0f;

	dest->y = 
		  self->scalars[0][1] * v.x + self->scalars[1][1] * v.y 
		+ self->scalars[2][1] * v.z + self->scalars[3][1] * 1.0f;

	dest->z = 
		  self->scalars[0][2] * v.x + self->scalars[1][2] * v.y 
		+ self->scalars[2][2] * v.z + self->scalars[3][2] * 1.0f;
}

void mat4f_scale(struct Mat4f* self, struct Vec3f* vec, struct Mat4f* dest)
{
	log_assert(self, "is NULL");
	log_assert(vec, "is NULL");
	log_assert(dest, "is NULL");

	dest->scalars[0][0] = self->scalars[0][0] * vec->x;
	dest->scalars[1][1] = self->scalars[1][1] * vec->y;
	dest->scalars[2][2] = self->scalars[2][2] * vec->z;
	//let w be unchanged
}

void mat4f_translate(struct Mat4f* self, struct Vec3f* vec, struct Mat4f* dest)
{
	log_assert(self, "is NULL");
	log_assert(vec, "is NULL");
	log_assert(dest, "is NULL");

	*dest = *self;
	dest->scalars[3][0] = vec->x;
	dest->scalars[3][1] = vec->y;
	dest->scalars[3][2] = vec->z;
}

void mat4f_rotate(
	struct Mat4f* self, 
	float angle, 
	struct Vec3f* vec, 
	struct Mat4f* dest
)
{
	log_assert(self, "is NULL");
	log_assert(vec, "is NULL");
	log_assert(dest, "is NULL");

	float cval = cosf(angle);
	float sval = sinf(angle);

	dest->scalars[0][0] = cval + vec->x * vec->x * (1 - cval);
	dest->scalars[1][0] = vec->x * vec->y * (1 - cval) - vec->z * sval;
	dest->scalars[2][0] = vec->x * vec->z * (1 - cval) + vec->y * sval;

	dest->scalars[0][1] = vec->y * vec->x * (1 - cval) + vec->z * sval;
	dest->scalars[1][1] = cval + vec->y * vec->y * (1 - cval);
	dest->scalars[2][1] = vec->y * vec->z * (1 - cval) - vec->x * sval;

	dest->scalars[0][2] = vec->z * vec->x * (1 - cval) - vec->y * sval;
	dest->scalars[1][2] = vec->z * vec->y * (1 - cval) + vec->x * sval;
	dest->scalars[2][2] = cval + vec->z * vec->z * (1 - cval);
}

