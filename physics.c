#include "physics.h"

static double clamp(double lower, double higher, double val);
static double expappr(double x);

// https://github.com/Brackeys/2D-Character-Controller/blob/master/CharacterController2D.cs

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
