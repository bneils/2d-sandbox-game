#include <SDL2/SDL.h>
#include <math.h>

#include "render.h"
#include "world.h"
#include "macros.h"

#define TILE_PATH(file_name) ("assets/" file_name)

// These must be ordered respective to the Tile enum in world.h
static const char *tile_filenames[] = {
	TILE_PATH("dirt.bmp"),
	TILE_PATH("grass.bmp"),
	NULL,
	TILE_PATH("log.bmp"),
};

static SDL_Surface *tile_surfaces[ARRAY_LEN(tile_filenames)];
static SDL_Surface *optimized_tile_surfaces[ARRAY_LEN(tile_surfaces)][2][2];

extern SDL_Surface *g_surface;

static int64_t world_to_pixel(double relative_tile_pos,
	double tile_width,
	double screen_midpoint)
{
	return floor(relative_tile_pos * tile_width + screen_midpoint);
}

/* `chunk_draw` draws an individual chunk at (x, y) on the screen */
static void chunk_draw(
	struct Chunk *chunk,
	struct PlayerView *view)
{
	// If the chunk doesn't exist, don't draw anything
	if (!chunk)
		return;

	double tile_width = SCREEN_WIDTH / view->width;

	for (int i = 0; i < CHUNK_LENGTH; ++i) {
		for (int j = 0; j < CHUNK_LENGTH; ++j) {
			enum Tile tile = chunk->tiles[i][j];

			// This first determines a tile's coordinate relative
			// to the visual center, then turns that into an offset
			// from the screen center
			int64_t px = world_to_pixel(
				chunk->cx * CHUNK_LENGTH + j - view->center_x,
				tile_width, SCREEN_WIDTH / 2.0);
			int64_t py = world_to_pixel(
				chunk->cy * CHUNK_LENGTH + i - view->center_y,
				tile_width, SCREEN_HEIGHT / 2.0);

			int64_t varied_tile_width = world_to_pixel(
				chunk->cx * CHUNK_LENGTH + j + 1 - view->center_x,
				tile_width, SCREEN_WIDTH / 2.0) - px;
			int64_t varied_tile_height = world_to_pixel(
				chunk->cy * CHUNK_LENGTH + i + 1 - view->center_y,
				tile_width, SCREEN_HEIGHT / 2.0) - py;

			SDL_Rect rect = {
				.x = px,
				.y = py,
				.w = varied_tile_width,
				.h = varied_tile_height,
			};

			// how much extra this sprite needs to fill in, besides
			// the average
			int extra_y = varied_tile_height - (int64_t) tile_width;
			int extra_x = varied_tile_width - (int64_t) tile_width;

			if (extra_y < 0 || extra_y > 1 || extra_x < 0 || extra_x > 1) {
				puts("Out of bounds");
				return;
			}

			SDL_Surface *tile_surface =
				optimized_tile_surfaces[tile][extra_y][extra_x];

			if (!tile_surface) {
				puts("No tile surface found");
				continue;
			}

			if (SDL_BlitSurface(tile_surface, NULL, g_surface, &rect) < 0)
				return;
		}
	}
}

/* `worldmap_draw` renders every chunk in-view according to the player view. */
void worldmap_draw(struct WorldMap *world, struct PlayerView *view)
{
	// (x1, y1) ---------------+ x2 > x1
	// |                       | y2 > y1
	// |   What the user can   |
	// |          see          |
	// +----------------(x2, y2)

	const double height = view->width *
		((double) SCREEN_HEIGHT / SCREEN_WIDTH);

	const double x1 = view->center_x - view->width / 2.0;
	const double x2 = x1 + view->width;
	const double y1 = view->center_y - height / 2.0;
	const double y2 = y1 + height;

	// Chunk coordinates to determine what chunks are within view
	const int64_t cx1 = floor(x1 / CHUNK_LENGTH);
	const int64_t cx2 = floor(x2 / CHUNK_LENGTH);
	const int64_t cy1 = floor(y1 / CHUNK_LENGTH);
	const int64_t cy2 = floor(y2 / CHUNK_LENGTH);

	for (int64_t cy = cy1; cy <= cy2; ++cy) {
		for (int64_t cx = cx1; cx <= cx2; ++cx) {
			struct Chunk *chunk = worldmap_get(world, cx, cy);
			if (!chunk)
				continue;
			chunk_draw(chunk, view);
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

/* `sprites_update` accepts a player view and optimizes surfaces for rendering.
 * This must be called after changing PlayerView.width to get correct results.
 */
void sprites_update(struct PlayerView *view)
{
	if (!view)
		return;

	int sprite_width = ceil(SCREEN_WIDTH / view->width);
	for (size_t i = 0; i < ARRAY_LEN(optimized_tile_surfaces); ++i) {
		for (int y = 0; y <= 1; ++y)
			for (int x = 0; x <= 1; ++x) {
				SDL_Surface *result =
					optimized_tile_surfaces[i][y][x] =
					SDL_CreateRGBSurfaceWithFormat(
						0, sprite_width + x,
						sprite_width + y,
						32, SDL_PIXELFORMAT_RGB888);
				SDL_BlitScaled(tile_surfaces[i], NULL,
					result, NULL);
			}
	}
}
