#ifndef DOBJECT_H
#define DOBJECT_H

#include "vector2d.h"

enum DObjectRegPoint
{
	DOBJECTREG_CENTER,
	DOBJECTREG_CENTERTOP,
	DOBJECTREG_CENTERLEFT,
	DOBJECTREG_CENTERRIGHT,
	DOBJECTREG_CENTERBOTTOM,

	DOBJECTREG_TOPLEFT,
	DOBJECTREG_TOPRIGHT,
	DOBJECTREG_BOTTOMLEFT,
	DOBJECTREG_BOTTOMRIGHT
};

//XXX: DObject is probably a really bad name
struct DObject
{
	struct Vector2d pos, oldpos;
	double width, height;
	enum DObjectRegPoint rpoint;
};

struct Vector2d dobject_getpos(struct DObject* self);
struct Vector2d dobject_getoldpos(struct DObject* self);
int dobject_intersects(struct DObject* self, struct DObject* dobj);
int dobject_intersectspoint(struct DObject* self, double x, double y);

#endif
