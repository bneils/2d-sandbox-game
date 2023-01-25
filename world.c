#include "world.h"
#include <stdlib.h>
#include <string.h>

/* https://web.archive.org/web/20220922000932/http://szudzik.com/ElegantPairing.pdf (pg.8)
 * Creates a deterministic hash from a pair of numbers,
 * similar to the Cantor pair function
 */
uint64_t hash_coordinate(int64_t x, int64_t y)
{
	uint64_t ux = x;
	uint64_t uy = y;

	return ux + ((ux >= uy) ? ux * ux + uy : uy * uy);
}

struct WorldMap *worldmap_new(void)
{
	struct WorldMap *world = malloc(sizeof(*world));
	if (!world)
		return NULL;

	world->chunk_buckets = calloc(WORLDMAP_BUCKETS_SIZE,
		sizeof(*world->chunk_buckets));

	if (!world->chunk_buckets) {
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

	uint64_t hash_idx = hash_coordinate(cx, cy) % WORLDMAP_BUCKETS_SIZE;
	struct Chunk *head = world->chunk_buckets[hash_idx];

	// Traverse the linked list and find a chunk that matches our coordinates
	while (head) {
		if (head->cx == cx && head->cy == cy)
			return head;
		head = head->next;
	}

	return NULL;
}

/* `worldmap_put` puts a (Chunk *) into a hash map-type structure if it does not
 * already exist. Iterates bucket linked list.
 */
void worldmap_put(struct WorldMap *world, struct Chunk *chunk)
{
	if (!world || !chunk)
		return;

	// Don't insert the chunk if it already exists
	if (worldmap_get(world, chunk->cx, chunk->cy))
		return;

	uint64_t hash_idx = hash_coordinate(chunk->cx, chunk->cy) % WORLDMAP_BUCKETS_SIZE;
	struct Chunk **head_ptr = &world->chunk_buckets[hash_idx];

	// Prepend to the beginning of this linked list
	chunk->next = *head_ptr;
	*head_ptr = chunk;
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

void chunk_fill(struct Chunk *chunk, enum Tile tile)
{
	for (uint64_t i = 0; i < CHUNK_LENGTH * CHUNK_LENGTH; ++i)
		((enum Tile *) chunk->tiles)[i] = tile;
}
