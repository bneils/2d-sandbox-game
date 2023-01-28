#include "entity.h"
#include <stdlib.h>

/**
 * Allocates and initializes an Entity structure
 * @param type The type of the entity
 * @param x The x coordinate of this entity
 * @param y The y coordinate of this entity
 * @param hitbox_width The width of the entity's hitbox
 * @param hitbox_height The height of the entity's hitbox
 * @return A pointer to the entity or NULL if an error occurred
 */
struct Entity *entity_new(
	enum EntityType type,
	double x, double y,
	double hitbox_width, double hitbox_height)
{
	struct Entity *entity = malloc(sizeof(*entity));
	if (!entity)
		return NULL;
	entity->type = type;
	entity->x = x;
	entity->y = y;
	entity->hitbox_width = hitbox_width;
	entity->hitbox_height = hitbox_height;
	uuid_generate(entity->uuid);
	return entity;
}

/**
 * Releases all resources created by entity_new
 * @param entity The entity to be freed
 */
void entity_free(struct Entity *entity)
{
	free(entity);
}


