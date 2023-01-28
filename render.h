#ifndef RENDER_H
#define RENDER_H

#include "world.h"

#define SCREEN_WIDTH (720*16/9)
#define SCREEN_HEIGHT 720

struct PlayerView {
	double center_x, center_y;
	// How many blocks are shown horizontally
	double width;
};

int render_init(void);
void render_free(void);
int sprites_update(struct PlayerView *);
int world_draw(World world, struct PlayerView *view);

#endif // RENDER_H
