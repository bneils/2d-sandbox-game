#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "exit.h"
#include "world.h"
#include "render.h"

static void destroy(void);

SDL_Window *g_window;
SDL_Surface *g_surface;
struct WorldMap *g_world;

int main(int argc, char **argv)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
		sdl_error(destroy);

	g_window = SDL_CreateWindow(
		"Minecroift", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (!g_window)
		sdl_error(destroy);

	g_surface = SDL_GetWindowSurface(g_window);
	if (!g_surface)
		sdl_error(destroy);

	if (render_init() < 0)
		sdl_error(destroy);

	g_world = worldmap_new();
	// populate world with chunks
	for (int cy = -1; cy <= 0; ++cy) {
		for (int cx = -1; cx <= 0; ++cx) {
			struct Chunk *chunk = chunk_new(cx, cy);
			enum Tile tile = ((cx < 0) ^ (cy < 0)) ? TILE_DIRT : TILE_GRASS;
			chunk_fill(chunk, tile);
			printf("%d v. %d\n", tile, chunk->tiles[0][0]);
			worldmap_put(g_world, chunk);
		}
	}

	struct PlayerView player_view = {
		.center_x = 0.0,
		.center_y = 0.0,
		.width = 3.0,
		.height = 3.0,
	};

	worldmap_draw(g_world, &player_view);
	SDL_UpdateWindowSurface(g_window);

	SDL_Event e;
	bool running = true;
	while (running && SDL_WaitEvent(&e)) {
		switch (e.type) {
			case SDL_QUIT:
				running = false;
				break;
		}
	}

	destroy();
	return 0;
}

/* This function destroys all resources made by the user and stops SDL */
static void destroy(void)
{
	SDL_FreeSurface(g_surface);
	g_surface = NULL;

	SDL_DestroyWindow(g_window);
	g_window = NULL;

	SDL_Quit();
}
