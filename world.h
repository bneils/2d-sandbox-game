#ifndef WORLD_H
#define WORLD_H

#include <stdint.h>
#include "hashmap.h"

// -x -> +x

// -y
// ...
// +y

// Chunks are square
#define CHUNK_LENGTH 16

enum Tile {
	TILE_DIRT = 0,
	TILE_GRASS,
	TILE_AIR,
	TILE_LOG,
};

struct Chunk {
	// These are chunk coordinates (adjacent chunks increment each
	// coordinate)
	union {
		struct {
			int64_t cx, cy;
		};
		// This is just an alias to the bytes representing cx and cy.
		const uint8_t cxy[16];
	};
	enum Tile tiles[CHUNK_LENGTH][CHUNK_LENGTH];
	struct Chunk *next;
};

struct WorldMap {
	struct HashMap *chunkmap;
};

size_t hash_coordinate(int64_t x, int64_t y);

struct Chunk *chunk_new(int64_t cx, int64_t cy);

struct WorldMap *worldmap_new(void);
struct Chunk *worldmap_get(struct WorldMap *, int64_t cx, int64_t cy);
int worldmap_put(struct WorldMap *, struct Chunk *);
void chunk_fill(struct Chunk *, enum Tile);

#endif // WORLD_H
