#include <stdio.h>
#include <SDL2/SDL.h>
#include "exit.h"

#define SCREEN_WIDTH 512
#define SCREEN_HEIGHT 512

int main(int argc, char **argv)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		sdl_error();
	}

	SDL_Quit();
	return 0;
}
