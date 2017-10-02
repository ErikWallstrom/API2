#ifndef GLTEXTURE_H
#define GLTEXTURE_H

typedef unsigned int GLTexture;

GLTexture* gltexture_ctor(GLTexture* self, const char* path);
void gltexture_dtor(GLTexture* self);

#endif
