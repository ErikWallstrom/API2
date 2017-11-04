#ifndef VEC3F_H
#define VEC3F_H

#include "vec2f.h"

struct Vec3f
{
	union 
	{
		float scalars[3];
		struct 
		{
			union
			{
				struct Vec2f xy;
				struct 
				{
					float x, y;
				};
			};
			float z;
		};
		struct 
		{
			float r, g, b;
		};
	};
};

void vec3f_add(struct Vec3f* self, struct Vec3f* vec, struct Vec3f* dest);
void vec3f_sub(struct Vec3f* self, struct Vec3f* vec, struct Vec3f* dest);
void vec3f_scale(struct Vec3f* self, float scalar, struct Vec3f* dest);
void vec3f_length(struct Vec3f* self, float* dest);
void vec3f_normalize(struct Vec3f* self, struct Vec3f* dest);
void vec3f_dot(struct Vec3f* self, struct Vec3f* vec, float* dest);
void vec3f_cross(struct Vec3f* self, struct Vec3f* vec, struct Vec3f* dest);

#endif 
