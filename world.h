#ifndef WORLD_H
#define WORLD_H

#include <stdint.h>

// -x -> +x

// -y
// ...
// +y

// Chunks are square
#define CHUNK_LENGTH 16
#define WORLDMAP_BUCKETS_SIZE 1024

enum Tile {
	TILE_DIRT,
	TILE_GRASS,
	TILE_AIR,
	TILE_LOG,
};

struct Chunk {
	enum Tile tiles[CHUNK_LENGTH][CHUNK_LENGTH];
	// These are chunk coordinates (adjacent chunks increment each coordinate)
	int64_t cx, cy;
	struct Chunk *next;
};

struct WorldMap {
	struct Chunk **chunk_buckets;
};

uint64_t hash_coordinate(int64_t x, int64_t y);

struct Chunk *chunk_new(int64_t cx, int64_t cy);

struct WorldMap *worldmap_new(void);
struct Chunk *worldmap_get(struct WorldMap *, int64_t cx, int64_t cy);
void worldmap_put(struct WorldMap *, struct Chunk *);
void chunk_fill(struct Chunk *, enum Tile);

#endif // WORLD_H
