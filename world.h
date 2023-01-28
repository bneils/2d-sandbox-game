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

typedef struct {
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
} *Chunk;

typedef struct {
	struct HashMap *chunkmap;
} *World;

Chunk chunk_new(int64_t cx, int64_t cy);
void chunk_fill(Chunk, enum Tile);

World world_new(void);
void world_free(World);
Chunk world_get(World, int64_t cx, int64_t cy);
int world_put(World, Chunk);

#endif // WORLD_H
