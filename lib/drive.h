#ifndef DRIVE_H
#define DRIVE_H
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
struct coordinate {
 float latitude;
 float longitude;
 float altitude;
};

struct rover_state {
 struct coordinate position;
 float heading_rad;
};

struct wheel_velocity {
 float left;
 float right;
};

enum drive_status {
 DRIVE_REACHED_TARGET = 0,
 DRIVE_INVALID_INPUT = -1,
 DRIVE_INVALID_COMMAND = -2,
 DRIVE_MAX_STEPS_EXCEEDED = -3
};


static float clampf(float value,float min , float max);

static float normlize_angle(float angle);

static bool coordinate_is_finite(const struct coordinate *coordinate);

static bool rover_is_valid(const struct rover_state *rover);
static struct wheel_velocity limit_wheel_velocities(struct wheel_velocity velocity);
static bool apply_wheel_velocities(struct rover_state *rover,struct wheel_velocity velocity);

enum drive_status drive_to_target(struct rover_state *rover,const struct coordinate *target);


#endif
