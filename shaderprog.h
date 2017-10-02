#ifndef SHADERPROG_H
#define SHADERPROG_H

typedef unsigned int ShaderProg;

ShaderProg* shaderprog_ctor(
	ShaderProg* self, 
	const char* vshaderpath,
	const char* fshaderpath
);
void shaderprog_setint(ShaderProg* self, const char* name, int value);
void shaderprog_setfloat(ShaderProg* self, const char* name, float value);
void shaderprog_dtor(ShaderProg* self);

#endif
