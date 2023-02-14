#include "../world.h"
#include "testing.h"

int main(void)
{
	World world = world_new();

	// Chunk does not exist yet
	assert(!world_get_chunk(world, 0, 0));

	// (0, 0) falls within chunk (0, 0)
	world_set_block(world, 0, 0, TILE_DIRT);

	// Setting the block here should've created a chunk
	assert(world_get_chunk(world, 0, 0));

	puts("passed");
	return 0;
}
