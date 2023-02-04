#ifndef ENTITY_H
#define ENTITY_H

#include <uuid/uuid.h>
#include <stdint.h>
#include <stdbool.h>

#define NUM_INVENTORY_SLOTS 10
#define PLAYER

enum EntityType {
	PLAYER_ENTITY,
	COW_ENTITY,
};

enum LookingDirection {
	LOOKING_LEFT,
	LOOKING_RIGHT,
};

enum ItemType {
	ITEM_NONE,
	ITEM_DIRT,
	ITEM_LOG,
};

struct Item {
	enum ItemType type;
	bool stackable;
	// If the item is stackable, quantity is used,
	// but if it isn't, durability is used instead
	union {
		uint8_t quantity;
		uint8_t durability;
	};
};

struct Player {
	struct Item inventory[NUM_INVENTORY_SLOTS];
};

struct Entity {
	double x, y;
	double hitbox_width, hitbox_height;
	double health;

	double velocity_x, velocity_y;
	double desired_velocity_x;
	double acceleration_x; // Don't modify, used for physics.c

	enum EntityType type;
	enum LookingDirection looking_dir;
	uuid_t uuid;
	// The following depends on what the entity type is
	union {
		struct Player player;
	};
};

typedef struct Entity *Entity;

Entity entity_new(enum EntityType, double x, double y, double hw, double hh,
	double health);
Entity entity_new_player(double x, double y);
void entity_free(Entity);
void entity_update_physics(Entity, float t);
uint32_t entity_hash(Entity);
bool entity_is_colliding(Entity, World);

#endif // ENTITY_H
