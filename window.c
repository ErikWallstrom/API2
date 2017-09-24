#include "window.h"
#include "log.h"

struct Window* window_ctor(
	struct Window* self, 
	const char* title, 
	int width, 
	int height,
	enum WindowFlags flags
)
{
	log_assert(self, "is NULL");
	log_assert(title, "is NULL");
	log_assert(width > 0, "invalid width");
	log_assert(height > 0, "invalid height");

	int isopengl = flags & WINDOW_OPENGL;
	int isvsync = flags & WINDOW_VSYNC;

	self->raw = SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width,
		height,
		(isopengl) ? SDL_WINDOW_OPENGL : SDL_WINDOW_SHOWN
	);

	if(!self->raw)
	{
		log_error("%s", SDL_GetError());
	}
	
	if(isopengl)
	{
		self->context = SDL_GL_CreateContext(self->raw);
		if(!self->context)
		{
			log_error("%s", SDL_GetError());
		}

		if(isvsync)
		{
			SDL_GL_SetSwapInterval(1); //Should this be -1?
		}
		else
		{
			SDL_GL_SetSwapInterval(0);
		}
	}
	else
	{
		int renderflags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
		if(isvsync)
		{
			renderflags |= SDL_RENDERER_PRESENTVSYNC;
		}

		self->renderer = SDL_CreateRenderer(self->raw, -1, renderflags);
		if(!self->renderer)
		{
			log_error("%s", SDL_GetError());
		}
	}

	self->title = SDL_GetWindowTitle(self->raw);
	self->flags = flags;
	self->width = width;
	self->height = height;
	self->fps = 0;
	self->frames = 0;
	self->seconds = SDL_GetTicks() / 1000;

	return self;
}

void window_render(struct Window* self)
{
	log_assert(self, "is NULL");
	if(self->flags & WINDOW_OPENGL)
	{
		SDL_GL_SwapWindow(self->raw);
	}
	else
	{
		SDL_RenderPresent(self->renderer);
		SDL_RenderClear(self->renderer);
	}

	self->frames++;
	if(SDL_GetTicks() / 1000 > self->seconds)
	{
		self->seconds = SDL_GetTicks() / 1000;
		self->fps = self->frames;
		self->frames = 0;
	}
}

void window_dtor(struct Window* self)
{
	log_assert(self, "is NULL");
	if(self->flags & WINDOW_OPENGL)
	{
		SDL_GL_DeleteContext(self->context);
	}
	else
	{
		SDL_DestroyRenderer(self->renderer);
	}

	SDL_DestroyWindow(self->raw);
}

