#include "gltexture.h"
#include "log.h"

#include <SDL2/SDL_image.h>
#include <GL/glew.h>

GLTexture* gltexture_ctor(GLTexture* self, const char* path)
{
	log_assert(self, "is NULL");
	log_assert(path, "is NULL");

	glGenTextures(1, self);
	glBindTexture(GL_TEXTURE_2D, *self);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	SDL_Surface* surface = IMG_Load(path);
	if(!surface)
	{
		log_error("%s", IMG_GetError());
	}

	int format;
	if(surface->format->BytesPerPixel == 4)
	{
		format = GL_RGBA;
	}
	else
	{
		format = GL_RGB;
	}

	glTexImage2D(
		GL_TEXTURE_2D, 
		0, 
		format, 
		surface->w, 
		surface->h, 
		0, 
		format, 
		GL_UNSIGNED_BYTE,
		surface->pixels
	);
	glGenerateMipmap(GL_TEXTURE_2D);

	SDL_FreeSurface(surface);
	return self;
}

void gltexture_dtor(GLTexture* self)
{
	log_assert(self, "is NULL");
	glDeleteTextures(1, self);
}

