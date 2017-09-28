#include "animation.h"
#include "log.h"

void animation_update(struct Animation* self, double tickrate)
{
	log_assert(self, "is NULL");

	self->time += tickrate;
	if(self->time - self->oldtime >= self->delay)
	{
		self->oldtime = self->time;
		self->frame++;

		if(self->numframes <= self->frame)
		{
			self->frame = 0;
		}
	}
}

