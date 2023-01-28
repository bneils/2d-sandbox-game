#include "entity.h"
#include <stdlib.h>

/* `entity_new` allocates and initializes an Entity structure */
struct Entity *entity_new(
	enum EntityType type,
	double x, double y,
	double hitw, double hith)
{
	struct Entity *entity = malloc(sizeof(*entity));
	if (!entity)
		return NULL;
	entity->type = type;
	entity->x = x;
	entity->y = y;
	entity->hitbox_width = hitw;
	entity->hitbox_height = hith;
	uuid_generate(entity->uuid);
	return entity;
}

void entity_free(struct Entity *entity)
{
	free(entity);
}


