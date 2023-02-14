#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "exit.h"
#include "event.h"
#include "entity.h"

extern Entity g_player;

void left_click_handler(void)
{

}

void event_handler(void)
{
	SDL_Event e;

	while (SDL_PollEvent(&e)) {
		switch (e.type) {
			case SDL_QUIT:
				destroy();
				exit(0);
				break;
			case SDL_MOUSEBUTTONDOWN:
				break;
		}
	}

	if (!g_player)
		return;

	int num_keys;
	const uint8_t *keystates = SDL_GetKeyboardState(&num_keys);

	if (keystates[SDL_SCANCODE_A]) {
		g_player->looking_dir = LOOKING_LEFT;
		g_player->desired_velocity_x = -5.0;
	} else if (keystates[SDL_SCANCODE_D]) {
		g_player->looking_dir = LOOKING_RIGHT;
		g_player->desired_velocity_x = 5.0;
	} else {
		g_player->desired_velocity_x = 0.0;
	}

	//if (g_player->on_ground && keystates[SDL_SCANCODE_SPACE]) {
	if (keystates[SDL_SCANCODE_SPACE]) {
		g_player->velocity_y = 5.0;
	}
}
