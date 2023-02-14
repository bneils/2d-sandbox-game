#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "world.h"
#include "entity.h"
#include "macros.h"
#include "globals.h"

/**
 * Hashes a pair of numbers using the Elegant Pairing function
 * http://szudzik.com/ElegantPairing.pdf (pg.8)
 * @param x The x coordinate
 * @param y The y coordinate
 * @return The hashed value
 */
size_t hash_coordinate(int64_t x, int64_t y)
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
	world->entitymap = hashmap_new(512);
	if (!world->entitymap) {
		hashmap_free(world->chunkmap);
		free(world);
		return NULL;
	}
	return world;
}

/**
 * Frees all resources allocated by world_new and all entities and chunks stored
 * within
 * @param world The world to destroy
 */
void world_free(World world)
{
	if (!world)
		return;

	// Free all chunks by iterating hash map
	struct HashMapIterator it;
	struct HashMapNode *entry;
	hashmap_iterator_init(&it, world->chunkmap);
	while ((entry = hashmap_iterate(&it)))
		chunk_free(entry->value);
	hashmap_free(world->chunkmap);

	hashmap_iterator_init(&it, world->entitymap);
	while ((entry = hashmap_iterate(&it)))
		entity_free(entry->value);
	hashmap_free(world->entitymap);
}

/**
 * Gets a world chunk
 * @param world The world to index
 * @param cx The chunk x-coordinate
 * @param cy The chunk y-coordinate
 * @return A pointer to the Chunk structure, or NULL if it doesn't exist
 */
Chunk world_get_chunk(World world, int64_t cx, int64_t cy)
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
int world_put_chunk(World world, Chunk chunk)
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
		g_error_message = "malloc failed";
		return NULL;
	}

	chunk->cx = cx;
	chunk->cy = cy;

	// Chunks by default contain only air
	chunk_fill(chunk, TILE_AIR);

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
	if (!world)
		return -1;

	const int x1 = -16 * CHUNK_LENGTH;
	const int x2 = -x1;
	const int y1 = -16 * CHUNK_LENGTH;
	const int y2 = 0;

	if (world_fill_block(world, x1, y1, x2, y2, TILE_DIRT) < 0)
		return -1;
	if (world_fill_block(world, x1, y2, x2, y2, TILE_GRASS) < 0)
		return -1;
	if (world_fill_block(world, x1, y1, x2, y1, TILE_UNBREAKABLE_ROCK) < 0)
		return -1;

	return 0;
}

/**
 * Change the block at (x, y) in the world
 * @param world The world
 * @param x The x-coordinate
 * @param y The y-coordinate
 * @param tile The tile to use
 * @return 0 on success or a negative value on error
 */
int world_set_block(World world, int64_t x, int64_t y, enum BlockID tile)
{
	int64_t cx = floor(x / (double) CHUNK_LENGTH);
	int64_t cy = floor(y / (double) CHUNK_LENGTH);
	Chunk chunk = world_get_chunk(world, cx, cy);
	if (!chunk) {
		chunk = chunk_new(cx, cy);
		if (!chunk)
			return -1;
		if (world_put_chunk(world, chunk) < 0)
			return -1;
	}
	int rx = x - cx * CHUNK_LENGTH;
	int ry = y - cy * CHUNK_LENGTH;
	chunk->tiles[ry][rx] = tile;
	return 0;
}

/**
 * Get the block at (x, y) in the world
 * @param world The world
 * @param x The x-coordinate
 * @param y The y-coordinate
 * @return The block ID, or an air block if the chunk doesn't exist
 */
enum BlockID world_get_block(World world, int64_t x, int64_t y)
{
	int64_t cx = floor(x / (double) CHUNK_LENGTH);
	int64_t cy = floor(y / (double) CHUNK_LENGTH);
	Chunk chunk = world_get_chunk(world, cx, cy);
	if (!chunk)
		return TILE_AIR;
	int rx = x - cx * CHUNK_LENGTH;
	int ry = y - cy * CHUNK_LENGTH;
	return chunk->tiles[ry][rx];
}

/**
 * Inserts an entity into the world
 * @param world The world
 * @param entity The entity to insert into the world
 * @return 0 on success or a negative value on error
 */
int world_put_entity(World world, Entity entity)
{
	return hashmap_put(world->entitymap, entity->uuid, sizeof(entity->uuid),
		entity, entity_hash(entity));
}

/**
 * Fills a region of blocks
 * @param world The world
 * @param x The x-coordinate of the top-left point
 * @param y The y-coordinate of the top-left point
 * @param w The width of the region
 * @param h The height of the region
 * @param block The BlockID to fill
 * @return 0 on success or a negative value on error
 */
int world_fill_block(World world, int64_t x1, int64_t y1, int64_t x2, int64_t y2,
	enum BlockID block)
{
	for (int64_t y = y1; y <= y2; ++y) {
		for (int64_t x = x1; x <= x2; ++x) {
			if (world_set_block(world, x, y, block) < 0)
				return -1;
		}
	}

	return 0;
}

/**
 * Releases all memory allocated for a chunk by chunk_new
 * @param chunk The chunk to free
 */
void chunk_free(Chunk chunk)
{
	free(chunk);
}


