#include "../../inputhandler.h"
#include "../../shaderprog.h"
#include "../../window.h"
#include "../../log.h"
#include <GL/glew.h>

#define defer(func) __attribute__((cleanup(func)))

//https://learnopengl.com/#!Getting-started/Textures

struct Game
{
	struct InputHandler* input;
	struct Window* window;
	ShaderProg* program;
	GLuint* vao;
	int* done;
	double delta;
};

static struct Game game;

static void handleinputs(double tickrate)
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

static void update(double tickrate)
{
	handleinputs(tickrate);
}

static void render(double tickrate, double adt)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(*game.program);
	glBindVertexArray(*game.vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	window_render(game.window);
}

int main(void)
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(
		SDL_GL_CONTEXT_PROFILE_MASK, 
		SDL_GL_CONTEXT_PROFILE_CORE
	);

	defer(window_dtor) struct Window window;
	window_ctor(&window, "Game Window", 800, 600, WINDOW_VSYNC | WINDOW_OPENGL);

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

	float vertices[] = {
		//Position		//Color
		-0.5f,  0.5f, 	1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  	0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f,  	0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  	1.0f, 1.0f, 1.0f,
	};

	GLuint indices[] = {
		0, 2, 1,
		0, 3, 1,
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

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), NULL);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		1, 
		3, 
		GL_FLOAT, 
		GL_FALSE, 
		5 * sizeof(float), 
		(void*)(2 * sizeof(float))
	);
	glEnableVertexAttribArray(1);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	double tickrate = 1000.0 / 30.0;
	Uint64 oldtime = SDL_GetPerformanceCounter();
	double adt = 0.0; //accumulated delta time
	int done = 0;

	game.program = &program;
	game.window = &window;
	game.input = &input;
	game.done = &done;
	game.vao = vao;

	while(!done)
	{
		game.delta = (double)((SDL_GetPerformanceCounter() - oldtime) * 1000)
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

