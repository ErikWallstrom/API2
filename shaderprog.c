#include "shaderprog.h"
#include "file.h"
#include "log.h"
#include <GL/glew.h>

typedef unsigned int VertexShader;
typedef unsigned int FragmentShader;

ShaderProg* shaderprog_ctor(
	ShaderProg* self, 
	const char* vshaderpath,
	const char* fshaderpath
)
{
	log_assert(self, "is NULL");
	log_assert(vshaderpath, "is NULL");
	log_assert(fshaderpath, "is NULL");

	VertexShader vshader = glCreateShader(GL_VERTEX_SHADER);
	struct File vshaderfile;
	file_ctor(&vshaderfile, vshaderpath, FILEMODE_READ);
	glShaderSource(
		vshader, 
		1, 
		(const char*[]){vshaderfile.content}, 
		NULL
	);
	glCompileShader(vshader);

	GLint success;
	glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		GLint size;
		glGetShaderiv(vshader, GL_INFO_LOG_LENGTH, &size);
		Vec(char) logbuffer = vec_ctor(char, size);
		glGetShaderInfoLog(vshader, size, NULL, logbuffer);
		log_error("In file \"%s\": %s", vshaderpath, logbuffer);
		vec_dtor(logbuffer); //Because why not
	}

	FragmentShader fshader = glCreateShader(GL_FRAGMENT_SHADER);
	struct File fshaderfile;
	file_ctor(&fshaderfile, fshaderpath, FILEMODE_READ);
	glShaderSource(
		fshader, 
		1, 
		(const char*[]){fshaderfile.content}, 
		NULL
	);
	glCompileShader(fshader);

	glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		GLint size;
		glGetShaderiv(fshader, GL_INFO_LOG_LENGTH, &size);
		Vec(char) logbuffer = vec_ctor(char, size);
		glGetShaderInfoLog(fshader, size, NULL, logbuffer);
		log_error("In file \"%s\": %s", fshaderpath, logbuffer);
		vec_dtor(logbuffer); //Because why not
	}

	ShaderProg program = glCreateProgram();
	glAttachShader(program, vshader);
	glAttachShader(program, fshader);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if(!success)
	{
		GLint size;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &size);
		Vec(char) logbuffer = vec_ctor(char, size);
		glGetProgramInfoLog(program, size, NULL, logbuffer);
		log_error("%s", logbuffer);
		vec_dtor(logbuffer); //Because why not
	}

	glValidateProgram(program);
	glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
	if(!success)
	{
		GLint size;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &size);
		Vec(char) logbuffer = vec_ctor(char, size);
		glGetProgramInfoLog(program, size, NULL, logbuffer);
		log_error("%s", logbuffer);
		vec_dtor(logbuffer); //Because why not
	}

	file_dtor(&vshaderfile);
	file_dtor(&fshaderfile);

	glDetachShader(program, vshader);
	glDetachShader(program, fshader);
	glDeleteShader(vshader);
	glDeleteShader(fshader);

	*self = program;
	return self;
}

void shaderprog_setint(ShaderProg* self, const char* name, int value)
{
	log_assert(self, "is NULL");
	log_assert(name, "is NULL");

	GLint location = glGetUniformLocation(*self, name);
	if(location == -1)
	{
		log_error("Unable to set uniform (%s)", name);
	}

	glUniform1i(location, value);
}

void shaderprog_setfloat(ShaderProg* self, const char* name, float value)
{
	log_assert(self, "is NULL");
	log_assert(name, "is NULL");

	GLint location = glGetUniformLocation(*self, name);
	if(location == -1)
	{
		log_error("Unable to set uniform (%s)", name);
	}

	glUniform1f(location, value);
}

void shaderprog_setmat4f(
	ShaderProg* self, 
	const char* name, 
	struct Mat4f* mat
)
{
	log_assert(self, "is NULL");
	log_assert(name, "is NULL");

	GLint location = glGetUniformLocation(*self, name);
	if(location == -1)
	{
		log_error("Unable to set uniform (%s)", name);
	}

	glUniformMatrix4fv(location, 1, GL_FALSE, (float*)mat->scalars);
}

void shaderprog_dtor(ShaderProg* self)
{
	log_assert(self, "is NULL");
	glDeleteProgram(*self);
}

