#include "exit.h"
#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>

/**
 * Raises an error created by SDL
 * @param exit_handler A function that is called when an error is raised
 */
void sdl_error(void (*exit_handler)(void))
{
	fprintf(stderr, "SDL error: %s\n", SDL_GetError());
	// This function has external responsibilities such as freeing resources
	// and shutting SDL down.
	if (exit_handler)
		exit_handler();
	exit(1);
}
