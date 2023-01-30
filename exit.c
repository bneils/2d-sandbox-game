#include "exit.h"
#include "render.h"
#include "world.h"
#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#include <time.h>

char *error_message;
extern SDL_Surface *g_surface;
extern SDL_Window *g_window;
extern World g_world;

/**
 * Raises an error created by SDL or another function and prints an error
 */
void raise_error(void)
{
	const char *sdl_err_msg = SDL_GetError();

	if (sdl_err_msg)
		fprintf(stderr, "SDL error: %s\n", sdl_err_msg);
	else
		fprintf(stderr, "Error: %s\n", error_message);
	// This function has external responsibilities such as freeing resources
	// and shutting SDL down
	destroy();
	exit(1);
}

/**
 * This function destroys all resources made by the user and stops SDL
 */
void destroy(void)
{
#ifdef DEBUG
	clock_t t;
	t = clock();
#endif // DEBUG

	SDL_FreeSurface(g_surface);
	g_surface = NULL;

	SDL_DestroyWindow(g_window);
	g_window = NULL;

	world_free(g_world);
	g_world = NULL;

	render_free();
	SDL_Quit();

#ifdef DEBUG
	t = clock() - t;
	printf("destroy() took %lf ms\n", 1000.0 * t / CLOCKS_PER_SEC);
#endif // DEBUG
}
