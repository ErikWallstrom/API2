#ifndef MAT4F_H
#define MAT4F_H

#include "vec3f.h"

struct Mat4f
{
	float scalars[4][4];
};

extern const struct Mat4f MAT4F_IDENTITYMATRIX;

struct Mat4f* mat4f_ctor(struct Mat4f* self, float val);
void mat4f_mulvec3f(struct Mat4f* self, struct Vec3f* vec, struct Vec3f* dest);
void mat4f_scale(struct Mat4f* self, struct Vec3f* vec, struct Mat4f* dest);
void mat4f_translate(struct Mat4f* self, struct Vec3f* vec, struct Mat4f* dest);
void mat4f_rotate(
	struct Mat4f* self, 
	float angle, 
	struct Vec3f* vec, 
	struct Mat4f* dest
);

#endif
