#ifndef ENTITY_H
#define ENTITY_H

#include <uuid/uuid.h>

enum EntityType {
	TYPE_PLAYER,
};

struct Entity {
	double x, y;
	double hitbox_width, hitbox_height;
	uuid_t uuid;
	enum EntityType type;
};

struct Entity *entity_new(enum EntityType,
	double x, double y, double hw, double hh);

void entity_free(struct Entity *);

#endif // ENTITY_H
