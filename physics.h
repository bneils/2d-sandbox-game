#ifndef PHYSICS_H
#define PHYSICS_H

#include <stdbool.h>
#include "entity.h"
#include "world.h"

double smooth_damp(
	double current,
	double desired,
	double *velocity,
	double smooth_time,
	double max_speed,
	double dt);

void entity_update_physics(Entity, World, double t);
bool entity_is_colliding(Entity entity, World world);

#endif // PHYSICS_H
