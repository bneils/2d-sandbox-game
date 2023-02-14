#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "entity.h"

#define PLAYER_HITBOX_WIDTH 0.5
#define PLAYER_HITBOX_HEIGHT 2.0
#define PLAYER_HEALTH 10.0
#define PLAYER_WALK_SPEED 7.0
#define COW_HITBOX_WIDTH 2.0
#define COW_HITBOX_HEIGHT 1.0
#define COW_HEALTH 7.5

uint32_t SuperFastHash(const char *, int len);

/**
 * Allocates and initializes an Entity structure
 * @param type The type of the entity
 * @param x The x coordinate of this entity
 * @param y The y coordinate of this entity
 * @param hitbox_width The width of the entity's hitbox
 * @param hitbox_height The height of the entity's hitbox
 * @return A pointer to the entity or NULL if an error occurred
 */
Entity entity_new(
	enum EntityType type,
	double x, double y,
	double hitbox_width, double hitbox_height,
	double health)
{
	// calloc is used as a safety measure
	Entity entity = calloc(1, sizeof(*entity));
	if (!entity)
		return NULL;
	entity->type = type;
	entity->x = x;
	entity->y = y;
	entity->health = health;
	entity->hitbox_width = hitbox_width;
	entity->hitbox_height = hitbox_height;
	entity->acceleration_x = 0.0;
	entity->desired_velocity_x = 0.0;
	entity->velocity_x = 0.0;
	entity->velocity_y = 0.0;
	entity->looking_dir = LOOKING_RIGHT;
	entity->on_ground = false;
	uuid_generate(entity->uuid);
	return entity;
}

/**
 * Releases all resources created by entity_new
 * @param entity The entity to be freed
 */
void entity_free(Entity entity)
{
	free(entity);
}

/**
 * Creates a player entity
 * @param x The player's x-coordinate
 * @param y The player's y-coordinate
 * @return A pointer to the entity, or NULL if an error occurred
 */
Entity entity_new_player(double x, double y)
{
	Entity entity = entity_new(PLAYER_ENTITY, x, y,
		PLAYER_HITBOX_WIDTH, PLAYER_HITBOX_HEIGHT, PLAYER_HEALTH);

	if (!entity)
		return NULL;

	for (int i = 0; i < NUM_INVENTORY_SLOTS; ++i)
		(entity->player).inventory[i] = (struct Item) {
			.type = ITEM_NONE,
			.quantity = 0,
		};
	return entity;
}

/**
 * Creates a cow entity
 * @param x The cow's x-coordinate
 * @param y The cow's y-coordinate
 * @return A pointer to the entity, or NULL if an error occurred
 */
Entity entity_new_cow(double x, double y)
{
	Entity entity = entity_new(COW_ENTITY, x, y,
		COW_HITBOX_WIDTH, COW_HITBOX_HEIGHT, COW_HEALTH);

	if (!entity)
		return NULL;

	/* custom initialization */

	return entity;
}

uint32_t entity_hash(Entity entity)
{
	return SuperFastHash((const char *) entity->uuid, sizeof(entity->uuid));
}

