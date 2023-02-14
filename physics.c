#include <math.h>
#include <stdbool.h>
#include "physics.h"
#include "entity.h"
#include "world.h"

static double clamp(double lower, double higher, double val);
static double expappr(double x);
static void entity_hitbox_blockrange(Entity entity,
	int64_t *x1, int64_t *x2,
	int64_t *y1, int64_t *y2);

/**
 * Uses a critical smooth dampening function to step a value in the direction of
 * a desired value in a smooth manner
 * @param current The current position of the value
 * @param desired Where the value should be
 * @param velocity The velocity of current as it approaches desired
 * @param smooth_time How much time is needed to smooth
 * @param max_speed A maximum speed at which the value can change
 * @param dt How much time has passed
 */
double smooth_damp(
	double current,
	double desired,
	double *velocity,
	double smooth_time,
	double max_speed,
	double dt)
{
	// Source: Game Programming Gems 4 Ch 1.10 (pg. 95)
	// Also, Unity c# reference.
	if (smooth_time < 0.0001)
		smooth_time = 0.0001;

	double omega = 2.0 / smooth_time;
	double x = omega * dt;
	// exp is a partial taylor series approximation for e^-x with its center
	// at a=0.5
	double exp = expappr(x);
	double change = current - desired;
	double original_desired = desired;
	double max_change = max_speed * smooth_time;

	// This creates a maximum smooth speed
	change = clamp(-max_change, max_change, change);

	// Now that change might be less, we want to change the desired value to
	// reflect that. BTW this is only used in the next_current calculation
	desired = current - change;
	double temp = (*velocity + omega * change) * dt;
	*velocity = (*velocity - omega * temp) * exp;

	double next_current = desired + (change + temp) * exp;

	// if the function ever overshoots past the desired, just snap it back
	// to the desired value
	if ((original_desired > current) == (next_current > original_desired)) {
		next_current = original_desired;
		*velocity = 0.0;
	}

	return next_current;
}

/**
 * Clamps a value such that it is always between [lower, higher]
 * @param low The lower value
 * @param high The higher value
 * @param The clamped value
 */
static double clamp(double low, double high, double val)
{
	if (val < low)
		return low;
	if (val > high)
		return high;
	return val;
}

/**
 * Approximates e^-x well for x>0
 * @param x The variable
 * @return The approximation
 */
static double expappr(double x)
{
	// The values of this function were obtained by using Wolfram Alpha
	// This is a partial Taylor Series
	double x2 = x * x;
	double x3 = x2 * x;
	double x4 = x2 * x2;

	return 1 / (
		0.06869671961250534 * x4 +
		0.13739343922501068 * x3 +
		0.5152253970937901 * x2 +
		0.9961024343813274 * x +
		1.000395979357109);
}

/**
 * Checks if an entity is colliding with a block's hitbox
 * @param The entity to check for collision
 * @param The world
 * @return Whether or not an entity is intersecting with a block hitbox
 */
bool entity_is_colliding(Entity entity, World world)
{
	if (!entity || !world)
		return false;

	int64_t x1 = floor(entity->x - entity->hitbox_width);
	int64_t x2 = floor(entity->x + entity->hitbox_width);
	int64_t y1 = floor(entity->y - entity->hitbox_height);
	int64_t y2 = floor(entity->y + entity->hitbox_height);

	for (int64_t y = y1; y <= y2; ++y)
		for (int64_t x = x1; x <= x2; ++x) {
			enum BlockID block = world_get_block(world, x, y);
			if (block != TILE_AIR)
				return true;
		}
	return false;
}

static void entity_hitbox_blockrange(Entity entity,
	int64_t *x1, int64_t *x2,
	int64_t *y1, int64_t *y2)
{
	*x1 = floor(entity->x - entity->hitbox_width / 2.0);
	*x2 = floor(entity->x + entity->hitbox_width / 2.0);
	*y1 = floor(entity->y);
	*y2 = floor(entity->y + entity->hitbox_height);
}

double entity_collcheck_ver(Entity entity, World world)
{
	int64_t x1, x2, y1, y2;
	if (!entity || !world)
		return 0.0;

	entity_hitbox_blockrange(entity, &x1, &x2, &y1, &y2);
	for (int64_t y = y1; y <= y2; ++y) {
		for (int64_t x = x1; x <= x2; ++x) {
			if (world_get_block(world, x, y) != TILE_AIR) {
				return y - y1 + modf(entity->y, NULL);
			}
		}
	}

	return 0.0;
}

double entity_collcheck_hor(Entity entity, World world)
{
	int64_t x1, x2, y1, y2;
	if (!entity || !world)
		return 0.0;

	entity_hitbox_blockrange(entity, &x1, &x2, &y1, &y2);
	for (int64_t x = x1; x <= x2; ++x) {
		for (int64_t y = y1; y <= y2; ++y) {
			if (world_get_block(world, x, y) != TILE_AIR) {
				return x - x1 + modf(
					entity->x - entity->hitbox_width / 2.0,
					NULL);
			}
		}
	}

	return 0.0;
}

/**
 * Changes the entity's velocity and position based on its current acceleration
 * @param entity The entity to move
 * @param t The amount of time to be elapsed
 */
void entity_update_physics(Entity entity, World world, double t)
{
	if (!entity)
		return;
	entity->velocity_x = smooth_damp(
		entity->velocity_x,
		entity->desired_velocity_x,
		&entity->acceleration_x,
		0.1,
		1000.0,
		t);
/*
	if (entity->on_ground && entity->velocity_y < 0) {
		entity->velocity_y = 0.0;
	} else {
		entity->velocity_y -= 1.0;
		if (entity->velocity_y < -5.0)
			entity->velocity_y = -5.0;
	}
*/
	entity->x += entity->velocity_x * t;
	entity->y += entity->velocity_y * t;
}
