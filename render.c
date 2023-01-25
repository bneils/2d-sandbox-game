#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h> // TODO: Remove

#include "render.h"
#include "world.h"

#define TILE_PATH(file_name) ("assets/" file_name)

#define ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))

// These must be ordered respective to the Tile enum in world.h
static const char *tile_filenames[] = {
	TILE_PATH("dirt.bmp"),
	TILE_PATH("grass.bmp"),
	NULL,
	TILE_PATH("log.bmp"),
};

static SDL_Surface *tile_surfaces[ARRAY_LEN(tile_filenames)];

extern SDL_Surface *g_surface;

static void chunk_draw(struct Chunk *chunk, int64_t x, int64_t y, struct PlayerView *view)
{
	// if the chunk doesn't exist, don't draw anything
	if (!chunk)
		return;
	int scaled_sprite_width = SCREEN_WIDTH / view->width;
	int scaled_sprite_height = SCREEN_HEIGHT / view->height;

	for (int i = 0; i < CHUNK_LENGTH; ++i) {
		for (int j = 0; j < CHUNK_LENGTH; ++j) {
			enum Tile tile = chunk->tiles[i][j];
			SDL_Surface *tile_surface = tile_surfaces[tile];

			if (!tile_surface)
				continue;

			// BUG: This needs to be scaled...
			SDL_Rect rect = {
				.x = x + j * scaled_sprite_width,
				.y = y + i * scaled_sprite_height,
				.w = scaled_sprite_width,
				.h = scaled_sprite_height,
			};

			//printf("chunk (%ld,%ld) coord (%d,%d) at pixel (%d,%d) to (%d,%d)\n", chunk->cx, chunk->cy, j, i, rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);

			if (SDL_BlitScaled(tile_surface, NULL, g_surface, &rect) < 0)
				return;
		}
	}
}

void worldmap_draw(struct WorldMap *world, struct PlayerView *view)
{
	// determine what chunks are within view
	double left_x = view->center_x - view->width / 2.0;
	double right_x = view->center_x + view->width / 2.0;
	double top_y = view->center_y - view->height / 2.0;
	double bottom_y = view->center_y + view->height / 2.0;

	int64_t cx_lo = floor(left_x / CHUNK_LENGTH);
	int64_t cx_hi = floor(right_x / CHUNK_LENGTH);
	int64_t cy_lo = floor(top_y / CHUNK_LENGTH);
	int64_t cy_hi = floor(bottom_y / CHUNK_LENGTH);

	//printf("(%ld,%ld) -> (%ld,%ld)\n", cx_lo, cy_lo, cx_hi, cy_hi);

	for (int64_t cy = cy_lo; cy <= cy_hi; ++cy) {
		for (int64_t cx = cx_lo; cx <= cx_hi; ++cx) {
			struct Chunk *chunk = worldmap_get(world, cx, cy);
			int64_t px = (cx * CHUNK_LENGTH - left_x) / view->width * SCREEN_WIDTH;
			int64_t py = (cy * CHUNK_LENGTH - top_y) / view->height * SCREEN_HEIGHT;

			chunk_draw(chunk, px, py, view);
		}
	}
}

/* `render_init` loads all assets */
int render_init(void)
{
	for (unsigned long i = 0; i < ARRAY_LEN(tile_filenames); ++i) {
		if (!tile_filenames[i])
			continue;
		SDL_Surface *surface = SDL_LoadBMP(tile_filenames[i]);
		if (!surface)
			return -1;
		tile_surfaces[i] = surface;
	}
	return 0;
}
