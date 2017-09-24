#ifndef RECT_H
#define RECT_H

#include "vector2d.h"

enum RectRegPoint
{
	RECTREGPOINT_CENTER,
	RECTREGPOINT_CENTERTOP,
	RECTREGPOINT_CENTERLEFT,
	RECTREGPOINT_CENTERRIGHT,
	RECTREGPOINT_CENTERBOTTOM,

	RECTREGPOINT_TOPLEFT,
	RECTREGPOINT_TOPRIGHT,
	RECTREGPOINT_BOTTOMLEFT,
	RECTREGPOINT_BOTTOMRIGHT
};

//Should it really be called Rect?
struct Rect
{
	struct Vector2d pos;
	double width, height;
};

struct Rect* rect_ctor(
	struct Rect* self, 
	struct Vector2d pos, 
	enum RectRegPoint rpoint, 
	double width, 
	double height
);

struct Vector2d rect_getpos(struct Rect* self, enum RectRegPoint rpoint);
void rect_setpos(
	struct Rect* self, 
	struct Vector2d pos, 
	enum RectRegPoint rpoint
);

int rect_intersects(struct Rect* self, struct Rect* other);
int rect_intersectspoint(struct Rect* self, double x, double y);

int rect_hitleft(struct Rect* self, double dx, struct Rect* other);
int rect_hitright(struct Rect* self, double dx, struct Rect* other);
int rect_hittop(struct Rect* self, double dy, struct Rect* other);
int rect_hitbottom(struct Rect* self, double dy, struct Rect* other);

#endif
