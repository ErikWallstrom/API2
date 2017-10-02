#ifndef SHADERPROG_H
#define SHADERPROG_H

#include "mat4f.h"

typedef unsigned int ShaderProg;

ShaderProg* shaderprog_ctor(
	ShaderProg* self, 
	const char* vshaderpath,
	const char* fshaderpath
);
void shaderprog_setint(ShaderProg* self, const char* name, int value);
void shaderprog_setfloat(ShaderProg* self, const char* name, float value);
void shaderprog_setmat4f(
	ShaderProg* self, 
	const char* name, 
	struct Mat4f* mat
);
void shaderprog_dtor(ShaderProg* self);

#endif
