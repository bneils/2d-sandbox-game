#include <SDL2/SDL.h>
#include <math.h>

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

/* `chunk_draw` draws an individual chunk at (x, y) on the screen */
static void chunk_draw(struct Chunk *chunk, int64_t x, int64_t y, struct PlayerView *view)
{
	// If the chunk doesn't exist, don't draw anything
	if (!chunk || !view)
		return;
	int scaled_sprite_width = SCREEN_WIDTH / view->width;
	int scaled_sprite_height = SCREEN_HEIGHT / view->height;

	for (int i = 0; i < CHUNK_LENGTH; ++i) {
		for (int j = 0; j < CHUNK_LENGTH; ++j) {
			enum Tile tile = chunk->tiles[i][j];
			SDL_Surface *tile_surface = tile_surfaces[tile];

			if (!tile_surface)
				continue;

			SDL_Rect rect = {
				.x = x + j * scaled_sprite_width,
				.y = y + i * scaled_sprite_height,
				.w = scaled_sprite_width,
				.h = scaled_sprite_height,
			};

			if (SDL_BlitScaled(tile_surface, NULL, g_surface, &rect) < 0)
				return;
		}
	}
}

/* `worldmap_draw` renders every chunk in-view according to the player view. */
void worldmap_draw(struct WorldMap *world, struct PlayerView *view)
{
	// determine what chunks are within view
	double left_x = view->center_x - view->width / 2.0;
	double right_x = view->center_x + view->width / 2.0;
	double top_y = view->center_y - view->height / 2.0;
	double bottom_y = view->center_y + view->height / 2.0;

	int64_t chunk_x_left = floor(left_x / CHUNK_LENGTH);
	int64_t chunk_x_right = floor(right_x / CHUNK_LENGTH);
	int64_t chunk_y_top = floor(top_y / CHUNK_LENGTH);
	int64_t chunk_y_bottom = floor(bottom_y / CHUNK_LENGTH);

	int scaled_sprite_width = SCREEN_WIDTH / view->width;
	int scaled_sprite_height = SCREEN_HEIGHT / view->height;
	int64_t py = SCREEN_HEIGHT
		* (chunk_y_top * CHUNK_LENGTH - top_y) / view->height;
	int64_t px_left = SCREEN_WIDTH
		* (chunk_x_left * CHUNK_LENGTH - left_x) / view->width;

	for (int64_t cy = chunk_y_top; cy <= chunk_y_bottom; ++cy) {
		int64_t px = px_left;
		for (int64_t cx = chunk_x_left; cx <= chunk_x_right; ++cx) {
			struct Chunk *chunk = worldmap_get(world, cx, cy);
			chunk_draw(chunk, px, py, view);
			px += scaled_sprite_width * CHUNK_LENGTH;
		}
		py += scaled_sprite_height * CHUNK_LENGTH;
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
