#ifndef SHADERPROG_H
#define SHADERPROG_H

#define shaderprog_setint(self, name, value) \
	glUniform1i(glGetUniformLocation(*self, name), value)
#define shaderprog_setfloat(self, name, value) \
	glUniform1f(glGetUniformLocation(*self, name), value)

typedef unsigned int ShaderProg;

ShaderProg* shaderprog_ctor(
	ShaderProg* self, 
	const char* vshaderpath,
	const char* fshaderpath
);
void shaderprog_dtor(ShaderProg* self);

#endif
