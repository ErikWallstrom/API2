#ifndef SHADERPROG_H
#define SHADERPROG_H

typedef unsigned int ShaderProg;

ShaderProg* shaderprog_ctor(
	ShaderProg* self, 
	const char* vshaderpath,
	const char* fshaderpath
);
void shaderprog_dtor(ShaderProg* self);

#endif
