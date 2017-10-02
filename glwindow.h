#ifndef GLWINDOW_H
#define GLWINDOW_H

#include <SDL2/SDL.h>

enum GLWindowFlags
{
	GLWINDOW_DEFAULT   = 0,
	GLWINDOW_VSYNC 	   = 1 << 0,
	GLWINDOW_MAXIMIZED = 1 << 1,
};

struct GLWindow
{
	SDL_Window* raw;
	SDL_GLContext* context;

	const char* title;
	int width, height;
	int flags;
	unsigned fps, frames, seconds;
};

struct GLWindow* glwindow_ctor(
	struct GLWindow* self, 
	const char* title, 
	int width, 
	int height,
	enum GLWindowFlags flags
);
void glwindow_render(struct GLWindow* self);
void glwindow_dtor(struct GLWindow* self);

#endif
