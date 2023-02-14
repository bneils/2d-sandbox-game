#ifndef GLOBALS_H
#define GLOBALS_H

#include <SDL2/SDL.h>
#include "entity.h"
#include "world.h"

extern SDL_Window *g_window;
extern SDL_Surface *g_surface;
extern World g_world;
extern Entity g_player;

// error_message is only ever set if the source of the error wasn't SDL, or the
// programmer (by passing invalid parameters)
// An example: malloc failing
extern char *g_error_message;

#endif // GLOBALS_H
