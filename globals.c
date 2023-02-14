#include <SDL2/SDL.h>
#include "world.h"
#include "entity.h"
// This exists to define symbols for usage in tests as well as the source files
// to avoid build errors.

SDL_Window *g_window;
SDL_Surface *g_surface;
World g_world;
Entity g_player; // don't double-free, world_free will free this

char *g_error_message;
