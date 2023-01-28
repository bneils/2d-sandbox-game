#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>

#include "exit.h"
#include "world.h"
#include "render.h"

SDL_Window *g_window;
SDL_Surface *g_surface;
struct WorldMap *g_world;

int main(void)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
		raise_error();

	g_window = SDL_CreateWindow(
		"NotMinecraft", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (!g_window)
		raise_error();

	g_surface = SDL_GetWindowSurface(g_window);
	if (!g_surface)
		raise_error();

	if (render_init() < 0)
		raise_error();

	g_world = world_new();
	if (!g_world)
		raise_error();

	// populate world with chunks
	for (int cy = -128; cy <= 128; ++cy) {
		for (int cx = -128; cx <= 128; ++cx) {
			struct Chunk *chunk = chunk_new(cx, cy);
			if (!chunk)
				raise_error();
			int dist = abs(cx) + abs(cy);
			enum Tile tile = dist % 2 ? TILE_LOG : TILE_GRASS;
			chunk_fill(chunk, tile);
			if (world_put(g_world, chunk) < 0)
				raise_error();
		}
	}

	struct PlayerView player_view = {
		.center_x = 0.0,
		.center_y = 0.0,
		.width = 25,
	};

	if (sprites_update(&player_view) < 0)
		raise_error();

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
			raise_error();
		if (world_draw(g_world, &player_view) < 0)
			raise_error();
		player_view.center_x += 2.0 / 60.0;
		if (SDL_UpdateWindowSurface(g_window) < 0)
			raise_error();
		SDL_Delay(1000 / 60);
		t += 1.0 / 120.0;
	}

	destroy();
	return 0;
}

