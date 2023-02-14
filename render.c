#include <SDL2/SDL.h>
#include <math.h>
#include <string.h>
#include "render.h"
#include "world.h"
#include "macros.h"
#include "hashmap.h"
#include "globals.h"

// These must be ordered respective to the BlockID enum in world.h
static const char *tile_filenames[NUM_TILES] = {
	"assets/dirt.bmp",
	"assets/grass.bmp",
	NULL,
	"assets/log.bmp",
	"assets/unbreakable_rock.bmp",
};

static SDL_Surface *block_textures[ARRAY_LEN(tile_filenames)];
static HashMap scaled_block_textures[ARRAY_LEN(tile_filenames)];

/**
 * Draws an entity on the screen
 * @param entity The entity to be drawn
 * @param view The player's view
 * @return 0 on success and a negative value on error
 */
int entity_draw(Entity entity, struct PlayerView *view)
{
	if (!entity || !view)
		return -1;

	double tile_width = SCREEN_WIDTH / view->width;

	int entity_width = tile_width * entity->hitbox_width;
	int entity_height = tile_width * entity->hitbox_height;

	SDL_Rect rect;
	rect.x = SCREEN_WIDTH / 2.0
		+ (entity->x - entity->hitbox_width / 2.0 - view->center_x)
		* tile_width;
	rect.y = SCREEN_HEIGHT / 2.0
		- (entity->y + entity->hitbox_height - view->center_y)
		* tile_width;

	rect.w = entity_width;
	rect.h = entity_height;

	SDL_Surface *surface = SDL_CreateRGBSurface(0, rect.w, rect.h, 32,
		0, 0, 0, 0);
	if (!surface)
		return -1;

	if (SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 255, 0)
		) < 0)
		return -1;

	if (SDL_BlitSurface(surface, NULL, g_surface, &rect) < 0)
		return -1;

	return 0;
}

/**
 * Draws an individual chunk on the screen
 * @param chunk The chunk to be drawn
 * @param view The player view used to determine the screen coordinates
 * @return 0 on success and a negative value on SDL error
 */
static int chunk_draw(
	Chunk chunk,
	struct PlayerView *view)
{
	// If the chunk doesn't exist, don't draw anything
	if (!chunk || !view)
		return 0;

	double tile_width = SCREEN_WIDTH / view->width;

	for (int i = 0; i < CHUNK_LENGTH; ++i) {
		for (int j = 0; j < CHUNK_LENGTH; ++j) {
			enum BlockID tile = chunk->tiles[i][j];

			if (tile == TILE_AIR)
				continue;

			double rx = chunk->cx * CHUNK_LENGTH + j - view->center_x;
			double ry = chunk->cy * CHUNK_LENGTH + i - view->center_y;

			int64_t px = SCREEN_WIDTH / 2.0 + rx * tile_width;
			int64_t py = SCREEN_HEIGHT / 2.0 - ry * tile_width;

			int var_tile_width =
				SCREEN_WIDTH / 2.0 + (rx + 1) * tile_width - px;
			int var_tile_height =
				py - (SCREEN_HEIGHT / 2.0 - (ry + 1) * tile_width);

			SDL_Rect rect = {
				.x = px,
				.y = py,
				.w = var_tile_width,
				.h = var_tile_height,
			};

			int key[] = {var_tile_width, var_tile_height};
			size_t hash = hash_coordinate(var_tile_width,
				var_tile_height);
			SDL_Surface **surface = (SDL_Surface **) hashmap_get(scaled_block_textures[tile],
				key, sizeof key, hash);

			if (!surface) {
				g_error_message = "scaled block texture not"
					" found in cache";
				return -1;
			}

			if (SDL_BlitSurface(*surface, NULL, g_surface,
				&rect) < 0)
				return -1;
		}
	}
	return 0;
}

/**
 * Renders every chunk in-view from the player's perspective
 * @param world The collection of chunks
 * @param view The player view
 * @return 0 on success and a negative value on SDL error
 */
int world_draw(World world, struct PlayerView *view)
{
	// (x1, y2) ---------------+ x2 > x1
	// |                       | y2 > y1
	// |   What the user can   |
	// |          see          |
	// +----------------(x2, y1)

	if (!world || !view)
		return -1;

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
			Chunk chunk = world_get_chunk(world, cx, cy);
			if (!chunk)
				continue;
			if (chunk_draw(chunk, view) < 0)
				return -1;
		}
	}

	struct HashMapIterator it;
	struct HashMapNode *entity_entry;
	hashmap_iterator_init(&it, world->entitymap);
	while ((entity_entry = hashmap_iterate(&it))) {
		Entity entity = entity_entry->value;
		// TODO: This won't render an entity if more than half of their
		// hitbox is out-of-frame
		if (x1 <= entity->x && entity->x <= x2 && y1 <= entity->y &&
			entity->y <= y2) {

			if (entity_draw(entity, view) < 0)
				return -1;
		}
	}

	return 0;
}

/**
 * Loads all assets from file
 * @return 0 on success and a negative value on SDL error
 */
int render_init(void)
{
	for (size_t i = 0; i < ARRAY_LEN(tile_filenames); ++i) {
		if (!tile_filenames[i])
			continue;
		SDL_Surface *surface = SDL_LoadBMP(tile_filenames[i]);
		if (!surface)
			return -1;
		block_textures[i] = surface;
		scaled_block_textures[i] = hashmap_new(8);
	}
	return 0;
}

/**
 * Frees all resources allocated by render_init
 */
void render_free(void)
{
	for (size_t i = 0; i < ARRAY_LEN(block_textures); ++i) {
		SDL_FreeSurface(block_textures[i]);

		struct HashMapNode *node;
		struct HashMapIterator it;
		hashmap_iterator_init(&it, scaled_block_textures[i]);
		while ((node = hashmap_iterate(&it)))
			SDL_FreeSurface(node->value);
		hashmap_free(scaled_block_textures[i]);
	}

	memset(block_textures, 0, sizeof(block_textures));
	memset(scaled_block_textures, 0, sizeof(scaled_block_textures));
}

/**
 * Optimizes surfaces for rendering
 * This must be called after changing PlayerView.width
 * @param view The player view
 * @return 0 on success and a negative value on SDL error
 */
int sprites_update(struct PlayerView *view)
{
	if (!view)
		return -1;

	int sprite_width = floor(SCREEN_WIDTH / view->width);
	for (size_t i = 0; i < ARRAY_LEN(block_textures); ++i) {
		// This means that either the block doesn't have a texture to
		// begin with, or render_init was never called
		if (!block_textures[i])
			continue;

		for (int h = sprite_width - 1; h <= sprite_width + 1; ++h)
			for (int w = sprite_width - 1; w <= sprite_width + 1; ++w) {
				// The image's pixel format changes, then it's
				// scaled
				SDL_Surface *formatted_surface =
					SDL_ConvertSurface(
						block_textures[i],
						g_surface->format,
						0);
				if (!formatted_surface)
					return -1;

				SDL_Surface *scaled_surface =
					SDL_CreateRGBSurface(
						0, w, h,
						formatted_surface->format->
							BitsPerPixel,
						formatted_surface->format->Rmask,
						formatted_surface->format->Gmask,
						formatted_surface->format->Bmask,
						formatted_surface->format->Amask
					);

				if (!scaled_surface) {
					SDL_FreeSurface(formatted_surface);
					return -1;
				}

				if (SDL_BlitScaled(formatted_surface, NULL,
					scaled_surface, NULL) < 0)
					return -1;

				SDL_FreeSurface(formatted_surface);

				size_t hash = hash_coordinate(w, h);
				// Using the address of w is a hack to skip a
				// dynamic allocation here. Put simply, &w is
				// right before &h, so the next int*2 bytes will
				// contain the dimensions of the surface.
				if (hashmap_put(scaled_block_textures[i],
					&scaled_surface->w, sizeof(int) * 2,
					scaled_surface, hash) < 0)
					return -1;
			}
	}

	return 0;
}
