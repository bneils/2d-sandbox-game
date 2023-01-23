#include "exit.h"
#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>

void sdl_error(void (*exit_handler)(void))
{
	fprintf(stderr, "error: %s\n", SDL_GetError());
	// This function has external responsibilities such as freeing resources
	if (exit_handler)
		exit_handler();
	SDL_Quit();
	exit(1);
}
