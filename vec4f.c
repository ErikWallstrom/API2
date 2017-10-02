#include "vec4f.h"
#include "log.h"

struct Vec4f* vec4f_ctor(
	struct Vec4f* self, 
	float x, 
	float y, 
	float z, 
	float w
)
{
	log_assert(self, "is NULL");

	self->x = x;
	self->y = y;
	self->z = z;
	self->w = w;

	return self;
}

