#ifndef PHYSICS_H
#define PHYSICS_H

double smooth_damp(
	double current,
	double desired,
	double *velocity,
	double smooth_time,
	double max_speed,
	double dt);

#endif // PHYSICS_H
