#include <stdlib.h>
#include <string.h>

#include "world.h"

/* `hash_coordinate` creates a deterministic hash from a pair of numbers,
 * similar to the Cantor pair function
 * https://web.archive.org/web/20220922000932/http://szudzik.com/ElegantPairing.pdf (pg.8)
 */
size_t hash_coordinate(int64_t x, int64_t y)
{
	uint64_t ux = x;
	uint64_t uy = y;

	return ux + ((ux >= uy) ? ux * ux + uy : uy * uy);
}

/* `worldmap_new` allocates all resources and initializes them for a WorldMap */
struct WorldMap *worldmap_new(void)
{
	struct WorldMap *world = malloc(sizeof(*world));
	if (!world)
		return NULL;
	world->chunkmap = hashmap_new(512);
	if (!world->chunkmap) {
		free(world);
		return NULL;
	}
	return world;
}

/* `worldmap_get` returns a (Chunk *) corresponding to a given (cx, cy).
 */
struct Chunk *worldmap_get(struct WorldMap *world, int64_t cx, int64_t cy)
{
	if (!world)
		return NULL;

	size_t hash = hash_coordinate(cx, cy);
	int64_t key[] = {cx, cy};

	struct Chunk **ptr = (struct Chunk **)
		hashmap_get(world->chunkmap, key, sizeof(key), hash);
	if (!ptr)
		return NULL;
	return *ptr;
}

/* `worldmap_put` puts a (Chunk *) into a hash map-type structure if it does not
 * already exist. Iterates bucket linked list.
 * Returns a negative value on error.
 */
int worldmap_put(struct WorldMap *world, struct Chunk *chunk)
{
	if (!world || !chunk)
		return -1;

	size_t hash = hash_coordinate(chunk->cx, chunk->cy);
	// The key and key_size fields happen to be the same size as cx & cy
	return hashmap_put(world->chunkmap, chunk->cxy, sizeof(chunk->cxy),
		chunk, hash);
}

/* `chunk_new` allocates a new Chunk structure and initializes it.
 */
struct Chunk *chunk_new(int64_t cx, int64_t cy)
{
	struct Chunk *chunk = malloc(sizeof(*chunk));
	if (!chunk)
		return NULL;

	chunk->next = NULL;
	chunk->cx = cx;
	chunk->cy = cy;

	return chunk;
}

/* `chunk_fill` replaces every Tile in a chunk with a specified tile. */
void chunk_fill(struct Chunk *chunk, enum Tile tile)
{
	if (!chunk)
		return;
	for (int i = 0; i < CHUNK_LENGTH; ++i)
		for (int j = 0; j < CHUNK_LENGTH; ++j)
			chunk->tiles[i][j] = tile;
}
