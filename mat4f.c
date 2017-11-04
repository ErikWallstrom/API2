#include "mat4f.h"
#include "log.h"

#include <math.h>

const struct Mat4f MAT4F_IDENTITY = {
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
	float angle, 
	struct Vec3f* vec, 
	struct Mat4f* dest
)
{
	log_assert(vec, "is NULL");
	log_assert(dest, "is NULL");

	float cval = cosf(angle);
	float sval = sinf(angle);
	float cvalminus = 1.0f - cval;

	dest->scalars[0][0] = vec->x * vec->x * cvalminus + cval;
	dest->scalars[1][0] = vec->x * vec->y * cvalminus + vec->z * sval;
	dest->scalars[2][0] = vec->x * vec->z * cvalminus - vec->y * sval;

	dest->scalars[0][1] = vec->y * vec->x * cvalminus - vec->z * sval;
	dest->scalars[1][1] = vec->y * vec->y * cvalminus + cval;
	dest->scalars[2][1] = vec->y * vec->z * cvalminus + vec->x * sval;

	dest->scalars[0][2] = vec->z * vec->x * cvalminus + vec->y * sval;
	dest->scalars[1][2] = vec->z * vec->y * cvalminus - vec->x * sval;
	dest->scalars[2][2] = vec->z * vec->z * cvalminus + cval;
}

//NOTE: Expects dest to already be set (for example to identity)
void mat4f_perspective(
	float fov, 
	float aspect, 
	float near, 
	float far, 
	struct Mat4f* dest
)
{
	log_assert(dest, "is NULL");
	
	float a = 1.0f / tanf(fov * 0.5f);

	dest->scalars[0][0] = a / aspect;
	dest->scalars[1][1] = a;
	dest->scalars[2][2] = (near + far) / (near - far);
	dest->scalars[2][3] = -1.0f;
	dest->scalars[3][2] = (2.0f * near * far) / (near - far);
}

//NOTE: Expects dest to already be set (for example to identity)
void mat4f_ortho(
	float left, 
	float right, 
	float bottom, 
	float top, 
	float near, 
	float far,
	struct Mat4f* dest
)
{
	log_assert(dest, "is NULL");

	dest->scalars[0][0] = 2.0f / (right - left);
	dest->scalars[1][1] = 2.0f / (top - bottom);
	dest->scalars[2][2] = 2.0f / (near - far);

	dest->scalars[3][0] = (left + right) / (left - right);
	dest->scalars[3][1] = (bottom + top) / (bottom - top);
	dest->scalars[3][2] = (far + near) / (near - far);
}

