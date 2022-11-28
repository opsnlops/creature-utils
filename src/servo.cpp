
#include <climits>
#include <cstdio>
#include <cstdlib>

#include "servo.h"


/**
 * Safely set up a new servo
 *
 * @param s a pointer to the servo object
 * @param joystick_axis which axis on the joystick to map this servo to
 * @param inverted true if the values should be inverted (up/down or left/right)
 */
void init_servo(servo *s, u_int8_t joystick_axis, bool inverted) {

    s->joystick_axis = joystick_axis;
    s->current_value = 0;
    s->requested_value = 127;
    s->inverted = inverted;

    printf("Paying attention to axis %d\n", s->joystick_axis);
}

/**
 * Set the requested value on a servo
 *
 * @param s the servo to change
 * @param requested_value the value to request
 */
void request_servo_value(servo *s, u_int8_t requested_value) {

    // If we need to invert this servo, do it now
    if(s->inverted) requested_value = UCHAR_MAX - requested_value - 1;

    s->requested_value = requested_value;
}

/**
 * Sets the current value of the servo to the requested value. Called after
 * we broadcast the value down the wire.
 *
 * @param s the servo to change
 */
void servo_requested_to_current(servo *s) {
    s->current_value = s->requested_value;
}

/**
 * Returns true if the servo value needs to be sent down the wire
 *
 * @param s the servo to look at
 * @return true if there's an update needed
 */
bool servo_needs_update(servo *s) {
    return (s->current_value != s->requested_value);
}