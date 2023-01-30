#ifndef WORLD_H
#define WORLD_H

#include <stdint.h>
#include <stdbool.h>
#include "hashmap.h"

// -x -> +x

// -y
// ...
// +y

// Chunks are square
#define CHUNK_LENGTH 16

enum BlockID {
	TILE_DIRT = 0,
	TILE_GRASS,
	TILE_AIR,
	TILE_LOG,
	TILE_UNBREAKABLE_ROCK,
	NUM_TILES
};

struct BlockProperty {
	float break_time_sec;
	bool has_collision;
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
	enum BlockID tiles[CHUNK_LENGTH][CHUNK_LENGTH];
} *Chunk;

typedef struct {
	HashMap chunkmap, entitymap;
} *World;

Chunk chunk_new(int64_t cx, int64_t cy);
void chunk_free(Chunk);
void chunk_fill(Chunk, enum BlockID);

World world_new(void);
void world_free(World);
Chunk world_get(World, int64_t cx, int64_t cy);
int world_put(World, Chunk);
int world_set_block(World, int64_t x, int64_t y, enum BlockID);

int world_generate_flat(World world);

#endif // WORLD_H
