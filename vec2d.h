#ifndef VEC2D_H
#define VEC2D_H

struct Vec2d
{
	union 
	{
		double scalars[2];
		struct 
		{
			double x, y;
		};
		struct 
		{
			double width, height;
		};
	};
};

struct Vec2d* vec2d_ctor(struct Vec2d* self, double x, double y);
void vec2d_add(struct Vec2d* self, struct Vec2d* vec, struct Vec2d* dest);
void vec2d_sub(struct Vec2d* self, struct Vec2d* vec, struct Vec2d* dest);
void vec2d_scale(struct Vec2d* self, double scalar, struct Vec2d* dest);
void vec2d_negate(struct Vec2d* self, struct Vec2d* dest);
void vec2d_length(struct Vec2d* self, double* dest);
void vec2d_normalize(struct Vec2d* self, struct Vec2d* dest);
void vec2d_lerp(
	struct Vec2d* self, 
	struct Vec2d* vec, 
	double alpha, 
	struct Vec2d* dest
);

#endif
