#include "../../inputhandler.h"
#include "../../shaderprog.h"
#include "../../gltexture.h"
#include "../../glwindow.h"
#include "../../mat4f.h"
#include "../../vec3f.h"
#include "../../log.h"

#include <GL/glew.h>

#define defer(func) __attribute__((cleanup(func)))
#define PI 3.1415926535897f
#define DEGREES(x) x * (180.0f / PI)
#define RADIANS(x) x * (PI / 180.0f)

struct Game
{
	struct InputHandler* input;
	struct GLWindow* window;
	struct Vec3f* positions;
	ShaderProg* program;
	GLTexture* texture1;
	GLTexture* texture2;
	GLuint* vao;
	int* done;
	float delta;
};

static struct Game game;

static void handleinputs(float tickrate)
{
	inputhandler_update(game.input);
	for(size_t i = 0; i < vec_getsize(game.input->events); i++)
	{
		switch(game.input->events[i].type)
		{
		case SDL_KEYDOWN:
			switch(game.input->events[i].key.keysym.sym)
			{
			case SDLK_w:
				break;
			case SDLK_s:
				break;
			case SDLK_a:
				break;
			case SDLK_d:
				break;
			default: 
				break;
			}
			break;

		case SDL_QUIT:
			*game.done = 1;
		}
	}
}

static void update(float tickrate)
{
	handleinputs(tickrate);
}

static void render(float tickrate, float adt)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *game.texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, *game.texture2);

	struct Mat4f view = MAT4F_IDENTITYMATRIX;
	mat4f_translate(&view, &(struct Vec3f){0.0f, 0.0f, -5.0f}, &view);

	struct Mat4f projection = MAT4F_IDENTITYMATRIX;
	mat4f_perspective(
		RADIANS(70.0f), 
		(float)game.window->width / game.window->height,
		0.1f,
		100.0f,
		&projection
	);

	glUseProgram(*game.program);
	shaderprog_setmat4f(game.program, "view", &view);
	shaderprog_setmat4f(game.program, "projection", &projection);

	glBindVertexArray(*game.vao);
	for(size_t i = 0; i < 10; i++)
	{
		struct Mat4f model = MAT4F_IDENTITYMATRIX;
		mat4f_translate(&model, &game.positions[i], &model);
		mat4f_rotate(RADIANS(50.0f) * SDL_GetTicks() / 500.0f, &(struct Vec3f){1.0f, 0.3f, 0.5f}, &model);
		shaderprog_setmat4f(game.program, "model", &model);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glwindow_render(game.window);
}

int main(void)
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(
		SDL_GL_CONTEXT_PROFILE_MASK, 
		SDL_GL_CONTEXT_PROFILE_CORE
	);

	defer(glwindow_dtor) struct GLWindow window;
	glwindow_ctor(
		&window, 
		"Game Window", 
		1200, 
		675, 
		GLWINDOW_VSYNC | GLWINDOW_FULLSCREEN
	);

	defer(inputhandler_dtor) struct InputHandler input;
	inputhandler_ctor(&input);

	glewExperimental = GL_TRUE;
	GLenum result = glewInit();
	if(result != GLEW_OK)
	{
		log_error("Error: %s", glewGetErrorString(result));
	}

	defer(shaderprog_dtor) ShaderProg program;
	shaderprog_ctor(&program, "vertex.glsl", "fragment.glsl");

	defer(gltexture_dtor) GLTexture texture1;
	gltexture_ctor(&texture1, "../2D_Game/res/player.png");

	defer(gltexture_dtor) GLTexture texture2;
	gltexture_ctor(&texture2, "../2D_Game/res/tree.png");

	/*
	float vertices[] = {
		//Position				//Texture
		-0.5f,  0.5f, 0.0f, 	0.0f, 0.0f, //Top left
		 0.5f,  0.5f, 0.0f, 	1.0f, 0.0f, //Top right
		-0.5f, -0.5f, 0.0f, 	0.0f, 1.0f, //Bottom left
		 0.5f, -0.5f, 0.0f, 	1.0f, 1.0f, //Bottom right
	};
	*/

	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	struct Vec3f cubePositions[] = {
		{0.0f,  0.0f,  0.0f}, 
		{2.0f,  5.0f, -15.0f}, 
		{-1.5f, -2.2f, -2.5f},  
		{-3.8f, -2.0f, -12.3f},  
		{2.4f, -0.4f, -3.5f},  
		{-1.7f,  3.0f, -7.5f},  
		{1.3f, -2.0f, -2.5f},  
		{1.5f,  2.0f, -2.5f}, 
		{1.5f,  0.2f, -1.5f}, 
		{-1.3f,  1.0f, -1.5f}  
	};

	GLuint indices[] = {
		0, 1, 2,
		1, 3, 2,
	};

	GLuint vao[1];
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);

	GLuint vbo[1];
	glGenBuffers(1, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint ebo[1];
	glGenBuffers(1, ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, 
		sizeof(indices), 
		indices, 
		GL_STATIC_DRAW
	);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), NULL);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		1, 
		2, 
		GL_FLOAT, 
		GL_FALSE, 
		5 * sizeof(float), 
		(void*)(3 * sizeof(float))
	);
	glEnableVertexAttribArray(1);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(program);
	shaderprog_setint(&program, "tex1", 0);
	shaderprog_setint(&program, "tex2", 1);

	Uint64 oldtime = SDL_GetPerformanceCounter();
	float tickrate = 1000.0f / 30.0f;
	float adt = 0.0f; //accumulated delta time
	int done = 0;

	game.positions = cubePositions;
	game.texture1 = &texture1;
	game.texture2 = &texture2;
	game.program = &program;
	game.window = &window;
	game.input = &input;
	game.done = &done;
	game.vao = vao;

	while(!done)
	{
		game.delta = (float)((SDL_GetPerformanceCounter() - oldtime) * 1000)
			/ SDL_GetPerformanceFrequency();
		adt += game.delta;
		oldtime = SDL_GetPerformanceCounter();

		while(adt >= tickrate) //Should this be if?
		{
			update(tickrate);
			adt -= tickrate;
		}

		render(tickrate, adt);

#ifndef NDEBUG
		static unsigned seconds = 0;
		if(seconds != window.seconds)
		{
			int length = snprintf(NULL, 0, "%s%i", "FPS: ", window.fps);
			char buffer[length + 1];
			snprintf(buffer, length + 1, "%s%i", "FPS: ", window.fps);

			//NOTE: SDL_SetWindowTitle is really slow
			SDL_SetWindowTitle(window.raw, buffer);
			seconds = window.seconds;
		}
#endif
	}

	glDeleteVertexArrays(1, vao);
	glDeleteBuffers(1, vbo);
}

