#include "glwindow.h"
#include "log.h"

struct GLWindow* glwindow_ctor(
	struct GLWindow* self, 
	const char* title, 
	int width, 
	int height,
	enum GLWindowFlags flags
)
{
	log_assert(self, "is NULL");
	log_assert(title, "is NULL");
	log_assert(width > 0, "invalid width");
	log_assert(height > 0, "invalid height");
	log_assert(flags <= GLWINDOW_FULLSCREEN, "invalid flag: %i", flags);

	self->raw = SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width,
		height,
		((flags & GLWINDOW_FULLSCREEN) 
		 	? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_SHOWN) 
			| SDL_WINDOW_OPENGL
	);
	if(!self->raw)
	{
		log_error("%s", SDL_GetError());
	}

	self->context = SDL_GL_CreateContext(self->raw);
	if(!self->context)
	{
		log_error("%s", SDL_GetError());
	}

	if(flags & GLWINDOW_VSYNC)
	{
		SDL_GL_SetSwapInterval(1); //Should this be -1?
	}
	else
	{
		SDL_GL_SetSwapInterval(0);
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

void glwindow_render(struct GLWindow* self)
{
	log_assert(self, "is NULL");
	SDL_GL_SwapWindow(self->raw);

	self->frames++;
	if(SDL_GetTicks() / 1000 > self->seconds)
	{
		self->seconds = SDL_GetTicks() / 1000;
		self->fps = self->frames;
		self->frames = 0;
	}
}

void glwindow_dtor(struct GLWindow* self)
{
	log_assert(self, "is NULL");

	SDL_GL_DeleteContext(self->context);
	SDL_DestroyWindow(self->raw);
}

