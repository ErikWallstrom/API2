#ifndef VECTOR2D_H
#define VECTOR2D_H

struct Vector2d
{
	double x, y;
};

struct Vector2d* vector2d_ctor(struct Vector2d* self, double x, double y);
void vector2d_add(
	struct Vector2d* self, 
	struct Vector2d* addend, //augend + addend = sum
	struct Vector2d* dest
);
void vector2d_subtract(
	struct Vector2d* self, 
	struct Vector2d* subtrahend, //minuend - subtrahend = difference
	struct Vector2d* dest
);
void vector2d_scale(
	struct Vector2d* self, 
	double scalar,
	struct Vector2d* dest
);
double vector2d_getlength(struct Vector2d* self);
void vector2d_normalize(struct Vector2d* self, struct Vector2d* dest);

#endif
