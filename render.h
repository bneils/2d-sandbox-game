#ifndef RENDER_H
#define RENDER_H

#include "world.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 512

struct PlayerView {
	double center_x, center_y;
	// How many blocks are shown horizontally
	double width;
};

int render_init(void);
void worldmap_draw(struct WorldMap *world, struct PlayerView *view);

#endif // RENDER_H
