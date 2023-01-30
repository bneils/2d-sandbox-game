#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "world.h"

static size_t hash_coordinate(int64_t, int64_t);

struct BlockProperty block_properties[NUM_TILES] = {
	{ .break_time_sec = 1.0, .has_collision = true }, // DIRT
	{ .break_time_sec = 1.0, .has_collision = true }, // GRASS
	{ .break_time_sec = INFINITY, .has_collision = false }, // AIR
	{ .break_time_sec = 5.0, .has_collision = false }, // LOG
	// UNBREAKABLE_ROCK
	{ .break_time_sec = INFINITY, .has_collision = true },
};

extern char *error_message;

/**
 * Hashes a pair of numbers using the Elegant Pairing function
 * http://szudzik.com/ElegantPairing.pdf (pg.8)
 * @param x The x coordinate
 * @param y The y coordinate
 * @return The hashed value
 */
static size_t hash_coordinate(int64_t x, int64_t y)
{
	uint64_t ux = x;
	uint64_t uy = y;

	return ux + ((ux >= uy) ? ux * ux + uy : uy * uy);
}

/**
 * Allocates and initializes all resources for a WorldMap
 * @return A pointer to the world, or NULL if an error occurred
 */
World world_new(void)
{
	World world = malloc(sizeof(*world));
	if (!world)
		return NULL;
	world->chunkmap = hashmap_new(512);
	if (!world->chunkmap) {
		free(world);
		return NULL;
	}
	return world;
}

/**
 * Frees all resources allocated by world_new
 * @param world The world to destroy
 */
void world_free(World world)
{
	if (!world)
		return;
	hashmap_free(world->chunkmap);
}

/**
 * Gets a world chunk
 * @param world The world to index
 * @param cx The chunk x-coordinate
 * @param cy The chunk y-coordinate
 * @return A pointer to the Chunk structure, or NULL if it doesn't exist
 */
Chunk world_get(World world, int64_t cx, int64_t cy)
{
	if (!world)
		return NULL;

	size_t hash = hash_coordinate(cx, cy);
	int64_t key[] = {cx, cy};

	Chunk *ptr = (Chunk *)
		hashmap_get(world->chunkmap, key, sizeof(key), hash);
	if (!ptr)
		return NULL;
	return *ptr;
}

/**
 * Inserts a chunk into the world
 * @param world The world to index
 * @param chunk The chunk structure to insert
 * @return 0 on success and a negative value on error
 */
int world_put(World world, Chunk chunk)
{
	if (!world || !chunk)
		return -1;

	size_t hash = hash_coordinate(chunk->cx, chunk->cy);
	// The key and key_size fields happen to be the same size as cx & cy
	return hashmap_put(world->chunkmap, chunk->cxy, sizeof(chunk->cxy),
		chunk, hash);
}

/**
 * Allocates a new Chunk structure and initializes it
 * @param cx The chunk x-coordinate
 * @param cy The chunk y-coordinate
 * @return A pointer to the chunk or NULL if an error occurred
 */
Chunk chunk_new(int64_t cx, int64_t cy)
{
	Chunk chunk = malloc(sizeof(*chunk));
	if (!chunk) {
		error_message = "malloc failed";
		return NULL;
	}

	chunk->cx = cx;
	chunk->cy = cy;

	return chunk;
}

/**
 * Fills every tile in a chunk with a specified BlockID
 * @param chunk The chunk to fill
 * @param tile The tile to use
 */
void chunk_fill(Chunk chunk, enum BlockID tile)
{
	if (!chunk)
		return;
	for (int i = 0; i < CHUNK_LENGTH; ++i)
		for (int j = 0; j < CHUNK_LENGTH; ++j)
			chunk->tiles[i][j] = tile;
}

/**
 * Generate a flat world with the ground at y=-1.
 * @param world The world to populate with chunks
 * @return 0 on success and a negative value on error
 */
int world_generate_flat(World world)
{
	for (int y = 0; y <= 16; ++y)
		for (int x = -16; x <= 16; ++x) {
			Chunk chunk = chunk_new(x, y);
			if (!chunk)
				return -1;
			if (world_put(world, chunk) < 0)
				return -1;
			chunk_fill(chunk,
				(y == 16) ? TILE_UNBREAKABLE_ROCK : TILE_DIRT
			);
		}
	return 0;
}
