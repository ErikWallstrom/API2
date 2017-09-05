#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <SDL2/SDL.h>
#include "vec.h"

struct InputHandler
{
	Vec(SDL_Event) events;
	const Uint8* keystate;
	Uint32 mousestate;
	int mousex, mousey;
};

struct InputHandler* inputhandler_ctor(struct InputHandler* self);
void inputhandler_update(struct InputHandler* self);

#endif
