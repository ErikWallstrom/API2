#ifndef ANIMATION_H
#define ANIMATION_H

#include <SDL2/SDL.h>

struct Animation
{
	//Use struct Rect instead?
	SDL_Rect* srects;
	double time, oldtime;
	double delay;
	size_t frame;
	size_t numframes;
};

void animation_update(struct Animation* self, double tickrate);

#endif
