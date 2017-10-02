#ifndef VEC4F_H
#define VEC4F_H

struct Vec4f
{
	float x, y, z;
	float w;
};

struct Vec4f* vec4f_ctor(
	struct Vec4f* self, 
	float x, 
	float y, 
	float z, 
	float w
);

#endif
