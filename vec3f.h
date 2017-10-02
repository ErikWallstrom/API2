#ifndef VEC3F_H
#define VEC3F_H

struct Vec3f
{
	float x, y, z;
};

struct Vec3f* vec3f_ctor(struct Vec3f* self, float x, float y, float z);
void vec3f_add(struct Vec3f* self, struct Vec3f* vec, struct Vec3f* dest);
void vec3f_sub(struct Vec3f* self, struct Vec3f* vec, struct Vec3f* dest);
void vec3f_scale(struct Vec3f* self, float scalar, struct Vec3f* dest);
void vec3f_negate(struct Vec3f* self, struct Vec3f* dest);
void vec3f_length(struct Vec3f* self, float* dest);
void vec3f_dot(struct Vec3f* self, struct Vec3f* vec, float* dest);
void vec3f_cross(struct Vec3f* self, struct Vec3f* vec, struct Vec3f* dest);

#endif 
