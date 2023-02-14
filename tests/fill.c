#include "../world.h"
#include "testing.h"

#define X1 -128
#define X2 128
#define Y1 -128
#define Y2 128
#define FILL_BLOCK TILE_DIRT

int main(void)
{
	World world = world_new();

	// The world is assumed to be empty, and then suddenly filled with dirt
	world_fill_block(world, X1, Y1, X2, Y2, FILL_BLOCK);

	// The whole area needs to be confirmed that it's filled
	for (int y = Y1; y <= Y2; ++y)
		for (int x = X1; x <= X2; ++x)
			assert(world_get_block(world, x, y) == FILL_BLOCK);

	puts("passed");
	return 0;
}
