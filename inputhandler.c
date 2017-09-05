#include "inputhandler.h"
#include "log.h"

struct InputHandler* inputhandler_ctor(struct InputHandler* self)
{
	log_assert(self, "is NULL");

	//TODO: Change 100 to a more appropriate value
	self->events = vec_ctor(SDL_Event, 100); 
	self->keystate = SDL_GetKeyboardState(NULL);
	self->mousestate = SDL_GetMouseState(&self->mousex, &self->mousey);

	return self;
}

void inputhandler_update(struct InputHandler* self)
{
	log_assert(self, "is NULL");
	vec_clear(self->events);

	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		vec_pushback(self->events, event);
	}

	self->mousestate = SDL_GetMouseState(&self->mousex, &self->mousey);
}


