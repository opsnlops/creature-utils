
#pragma once

#include <cstdio>
#include <unistd.h>
#include <cstdlib>

typedef struct {
    u_int8_t joystick_axis;             // The axis on the joystick we're mapped to
    u_int8_t current_value;             // What value we last sent
    u_int8_t requested_value;           // What value we want to be set to
    bool inverted;                      // Is up down? Is left right?
} servo;

void init_servo(servo *s, u_int8_t joystick_axis, bool inverted);
void request_servo_value(servo *s, u_int8_t requested_value);
void servo_requested_to_current(servo *s);
bool servo_needs_update(servo *s);
