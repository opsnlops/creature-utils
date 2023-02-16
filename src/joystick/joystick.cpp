
#include <fcntl.h>
#include <cassert>
#include <climits>
#include <cstdio>
#include <unistd.h>
#include <linux/joystick.h>

#include "log.h"

#include "common/config.h"

#include "joystick/joystick.h"
#include "joystick/servo.h"


extern Config* config;
extern int joystick_fd;
extern int second_joystick_fd;
extern servo servos[NUM_SERVOS];


int open_joystick(const char *device) {

    int fd = open(device, O_RDONLY);

    if (fd == -1) {
        log_fatal("Could not open joystick on %s", device);
        exit(0);
}

    return fd;
}


void close_joystick(int fd) {
    close(fd);
}


int read_joystick_event(int fd, struct js_event *event) {
    ssize_t bytes;

    bytes = read(fd, event, sizeof(*event));

    if (bytes == sizeof(*event))
        return 0;

    /* Error, could not read full event. */
    return -1;
}

/**
 * @brief Extracts the axis event and converts to our byte format
 *
 * @param event the event to look at
 */
ssize_t js_get_axis_event_value(struct js_event *event) {


    // TODO: This is pretty silly. Do this with bit-shifts instead.

    // Make sure this is an axis event
    assert(event->type == JS_EVENT_AXIS);

    // Convert to an unsigned int
    u_int16_t value = event->value + SHRT_MAX;
    float percent_used = (float)value / (float)USHRT_MAX;

    // Map to the range 0-255
    ssize_t dmx_value = UCHAR_MAX * percent_used;

    // Log the event
    log_debug("Processed event %d %d (%d -> %zd)", event->number, event->value, value, dmx_value);

    return dmx_value;
}



void *joystick_reader_thread(void *ptr)
{
    js_event event = {};

    // Wait for an event and update the array of servos if it's
    // one of the axes we care about.
    while (read_joystick_event(joystick_fd, &event) == 0) {

        // Toss something into the firehose
        log_trace("JS event: type: %d, number: %d, value: %d", event.type, event.number, event.value);

        // If this is an axis event, process it
        if (event.type == JS_EVENT_AXIS) {

            // Walk the servo list and see if this is one we care about
            for(auto & servo : servos) {
                if(servo.joystick_axis == event.number)
                {
                    request_servo_value(&servo, js_get_axis_event_value(&event));
                    break;
                }
            }
        }

        // Ignore the other event types for now (buttons and init)

    }

    return nullptr;
}

void *second_joystick_reader_thread(void *ptr)
{
    js_event event = {};

    // Wait for an event and update the array of servos if it's
    // one of the axes we care about.
    while (read_joystick_event(second_joystick_fd, &event) == 0) {

        // Toss something into the firehose
        log_trace("JS event: type: %d, number: %d, value: %d", event.type, event.number, event.value);

        // If this is an axis event, process it
        if (event.type == JS_EVENT_AXIS) {

            // Walk the servo list and see if this is one we care about
            for(auto & servo : servos) {
                if(servo.joystick_axis == event.number + 6)
                {
                    request_servo_value(&servo, js_get_axis_event_value(&event));
                    break;
                }
            }
        }

        // Ignore the other event types for now (buttons and init)

    }

    return nullptr;
}