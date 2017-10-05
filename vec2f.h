#ifndef VEC2F_H
#define VEC2F_H

struct Vec2f 
{
	union 
	{
		float scalars[2];
		struct 
		{
			float x, y;
		};
		struct 
		{
			float width, height;
		};
	};
};

void vec2f_add(struct Vec2f* self, struct Vec2f* vec, struct Vec2f* dest);
void vec2f_sub(struct Vec2f* self, struct Vec2f* vec, struct Vec2f* dest);
void vec2f_scale(struct Vec2f* self, float scalar, struct Vec2f* dest);
void vec2f_length(struct Vec2f* self, float* dest);
void vec2f_normalize(struct Vec2f* self, struct Vec2f* dest);

#endif
