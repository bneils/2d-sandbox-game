#include <math.h>
#include <stdbool.h>
#include <stdio.h> // TENTATIVE
#include "physics.h"
#include "entity.h"
#include "world.h"

static double clamp(double lower, double higher, double val);
static double expappr(double x);
static void entity_hitbox_blockrange(Entity entity,
	double *x1, double *x2,
	double *y1, double *y2);

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
	double *x1, double *x2,
	double *y1, double *y2)
{
	*x1 = entity->x - entity->hitbox_width / 2.0;
	*x2 = entity->x + entity->hitbox_width / 2.0;
	*y1 = entity->y;
	*y2 = entity->y + entity->hitbox_height;
}

double entity_collcheck_ver(Entity entity, World world)
{
	double x1, x2, y1, y2;
	if (!entity || !world)
		return 0.0;

	// Determine the smallest shift required to
	// uncollide the player vertically.
	entity_hitbox_blockrange(entity, &x1, &x2, &y1, &y2);

	int64_t ix1 = floor(x1);
	int64_t ix2 = floor(x2);
	int64_t iy1 = floor(y1);
	int64_t iy2 = floor(y2);

	double best_shift = INFINITY;

	for (int64_t y = iy1; y <= iy2; ++y) {
		for (int64_t x = ix1; x <= ix2; ++x) {
			if (world_get_block(world, x, y) == TILE_AIR)
				continue;
			double shift_down = y1 - y + entity->hitbox_height;
			double shift_up = y - y1 + 1;

			if (fabs(shift_down) < fabs(best_shift))
				best_shift = shift_down;

			if (fabs(shift_up) < fabs(best_shift))
				best_shift = shift_up;
			break;
		}
	}

	// No collision found
	if (best_shift == INFINITY)
		return 0.0;

	// A collision was found, this is the best course of action
	return best_shift;
}

double entity_collcheck_hor(Entity entity, World world)
{
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

	// Simple gravity and terminal velocity
	entity->velocity_y -= t * 9.81;
	if (entity->velocity_y < -9.81)
		entity->velocity_y = -9.81;

	entity->y += entity->velocity_y * t;
	entity->y += entity_collcheck_ver(entity, world);
}
