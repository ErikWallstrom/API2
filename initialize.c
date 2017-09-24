#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>
#include "log.h"

//Should these be called explicitly?
__attribute__((constructor))
static void initialize(void)
{
	srand(time(NULL));
	if(SDL_Init(SDL_INIT_EVERYTHING))
	{
		log_error("%s", SDL_GetError());
	}

	if(!IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG))
	{
		log_error("%s", IMG_GetError());
	}

	if(TTF_Init())
	{
		log_error("%s", TTF_GetError());
	}
}

__attribute__((destructor))
static void cleanup(void)
{
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

