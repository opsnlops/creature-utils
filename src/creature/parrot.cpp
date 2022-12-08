
#include "parrot.h"

#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <cassert>
#include <climits>

#include "log.h"


/*
 * This file defines the behavior of the parrot itself when being
 * controlled by a joystick.
 */


// Safely default to the middle of the range
u_int16_t neck_left_value       = USHRT_MAX / 2;
u_int16_t neck_right_value      = USHRT_MAX / 2;
u_int16_t neck_rotate_value     = USHRT_MAX / 2;
u_int16_t body_lean_value       = USHRT_MAX / 2;
u_int16_t beak_value            = USHRT_MAX / 2;
u_int16_t chest_value           = USHRT_MAX / 2;
u_int16_t stand_rotate_value    = USHRT_MAX / 2;


int parrot_value_from_joystick(u_int8_t axis, u_int16_t value)
{
    log_debug("event from joystick, axis: %d, value: %d", axis, value);

    if(axis > NUMBER_OF_SERVOS) {
        log_error("axis (%d) was greater than the number of servos (%d)", axis, NUMBER_OF_SERVOS);
        return 0;
    }



    return 1;
}