#include "dobject.h"
#include "log.h"

struct Vector2d dobject_getpos(struct DObject* self)
{
	log_assert(self, "is NULL");
	log_assert(
		self->rpoint <= DOBJECTREG_BOTTOMRIGHT, 
		"invalid rpoint (%i)", 
		self->rpoint
	);

	struct Vector2d pos;
	switch(self->rpoint)
	{
	case DOBJECTREG_CENTER:
		pos.x = self->pos.x - self->width / 2.0;
		pos.y = self->pos.y - self->height / 2.0;
		break;
	case DOBJECTREG_CENTERTOP:
		pos.x = self->pos.x - self->width / 2.0;
		pos.y = self->pos.y;
		break;
	case DOBJECTREG_CENTERLEFT:
		pos.x = self->pos.x;
		pos.y = self->pos.y - self->height / 2.0;
		break;
	case DOBJECTREG_CENTERRIGHT:
		pos.x = self->pos.x - self->width;
		pos.y = self->pos.y - self->height / 2.0;
		break;
	case DOBJECTREG_CENTERBOTTOM:
		pos.x = self->pos.x - self->width / 2.0;
		pos.y = self->pos.y - self->height;
		break;
	case DOBJECTREG_TOPLEFT:
		pos.x = self->pos.x;
		pos.y = self->pos.y;
		break;
	case DOBJECTREG_TOPRIGHT:
		pos.x = self->pos.x - self->width;
		pos.y = self->pos.y;
		break;
	case DOBJECTREG_BOTTOMLEFT:
		pos.x = self->pos.x;
		pos.y = self->pos.y - self->height;
		break;
	case DOBJECTREG_BOTTOMRIGHT:
		pos.x = self->pos.x - self->width;
		pos.y = self->pos.y - self->height;
		break;
	}

	return pos;
}

struct Vector2d dobject_getoldpos(struct DObject* self)
{
	log_assert(self, "is NULL");
	log_assert(
		self->rpoint <= DOBJECTREG_BOTTOMRIGHT, 
		"invalid rpoint (%i)", 
		self->rpoint
	);

	struct Vector2d pos;
	switch(self->rpoint)
	{
	case DOBJECTREG_CENTER:
		pos.x = self->pos.x - self->width / 2.0;
		pos.y = self->pos.y - self->height / 2.0;
		break;
	case DOBJECTREG_CENTERTOP:
		pos.x = self->pos.x - self->width / 2.0;
		pos.y = self->pos.y;
		break;
	case DOBJECTREG_CENTERLEFT:
		pos.x = self->pos.x;
		pos.y = self->pos.y - self->height / 2.0;
		break;
	case DOBJECTREG_CENTERRIGHT:
		pos.x = self->pos.x - self->width;
		pos.y = self->pos.y - self->height / 2.0;
		break;
	case DOBJECTREG_CENTERBOTTOM:
		pos.x = self->pos.x - self->width / 2.0;
		pos.y = self->pos.y - self->height;
		break;
	case DOBJECTREG_TOPLEFT:
		pos.x = self->pos.x;
		pos.y = self->pos.y;
		break;
	case DOBJECTREG_TOPRIGHT:
		pos.x = self->pos.x - self->width;
		pos.y = self->pos.y;
		break;
	case DOBJECTREG_BOTTOMLEFT:
		pos.x = self->pos.x;
		pos.y = self->pos.y - self->height;
		break;
	case DOBJECTREG_BOTTOMRIGHT:
		pos.x = self->pos.x - self->width;
		pos.y = self->pos.y - self->height;
		break;
	}

	return pos;
}

static int intersectsaxis(
	double amin, 
	double amax, 
	double bmin, 
	double bmax
)
{
	if(bmin > amin)
	{
		amin = bmin;
	}

	if(bmax < amax)
	{
		amax = bmax;
	}

	if(amax <= amin)
	{
		return 0;
	}

	return 1;
}

int dobject_intersects(struct DObject* self, struct DObject* dobj)
{
	log_assert(self, "is NULL");
	log_assert(dobj, "is NULL");

	struct Vector2d pos1 = dobject_getpos(self);
	struct Vector2d pos2 = dobject_getpos(dobj);

	int intersectx = intersectsaxis(
		pos1.x, 
		pos1.x + self->width, 
		pos2.x, 
		pos2.x + dobj->width
	);

	if(!intersectx)
	{
		return 0;
	}

	int intersecty = intersectsaxis(
		pos1.y,
		pos1.y + self->height, 
		pos2.y, 
		pos2.y + dobj->height
	);

	if(!intersecty)
	{
		return 0;
	}

	return 1;
}

int dobject_intersectspoint(struct DObject* self, double x, double y)
{
	log_assert(self, "is NULL");

	struct Vector2d pos = dobject_getpos(self);

	if((x >= pos.x) && (x <= (pos.x + self->width - 1)) && 
		(y >= pos.y) && (y <= (pos.y + self->height - 1)))
	{
		return 1;
	}

	return 0;
}

