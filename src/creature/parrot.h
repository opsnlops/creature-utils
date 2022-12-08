
#pragma once

#include <cstdlib>

#include "log.h"


// Turn on color in the logging library
#define LOG_USE_COLOR   1

#define NUMBER_OF_SERVOS 7

// Servo names
#define NECK_LEFT       0
#define NECK_RIGHT      1
#define NECK_ROTATE     2
#define BODY_LEAN       3
#define BEAK            4
#define CHEST           5
#define STAND_ROTATE    6


int parrot_value_from_joystick(u_int8_t axis, u_int16_t value);
