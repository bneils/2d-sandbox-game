#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>

#include "exit.h"
#include "world.h"
#include "render.h"

static void destroy(void);

SDL_Window *g_window;
SDL_Surface *g_surface;
struct WorldMap *g_world;

int main(void)
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
	for (int cy = -128; cy <= 128; ++cy) {
		for (int cx = -128; cx <= 128; ++cx) {
			struct Chunk *chunk = chunk_new(cx, cy);
			int dist = abs(cx) + abs(cy);
			enum Tile tile = dist % 2 ? TILE_LOG : TILE_GRASS;
			chunk_fill(chunk, tile);
			worldmap_put(g_world, chunk);
		}
	}

	struct PlayerView player_view = {
		.center_x = 0.0,
		.center_y = 0.0,
		.width = 2500,
	};

	sprites_update(&player_view);

	SDL_Event e;
	bool running = true;

	double t = 0;

	while (running) {
		if (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_QUIT:
					running = false;
					break;
			}
		}
		if (SDL_FillRect(g_surface, NULL, SDL_MapRGB(g_surface->format, 255, 0, 0)) < 0)
			sdl_error(destroy);
		worldmap_draw(g_world, &player_view);
		player_view.center_x += 2.0 / 60.0;
		SDL_UpdateWindowSurface(g_window);
		SDL_Delay(1000/60);
		t += 1.0 / 120.0;
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
