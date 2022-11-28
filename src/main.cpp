
#include <cstdio>
#include <linux/joystick.h>

#include "joystick.h"

int main() {
    int fd = open_joystick(JOYSTICK_DEVICE);

    js_event event = {};
    ssize_t value = 0;

    /* This loop will exit if the controller is unplugged. */
    while (read_joystick_event(fd, &event) == 0) {

        // If this is an axis event, process it
        if (event.type == JS_EVENT_AXIS) {
            value = js_get_axis_event_value(&event);
            printf("%zd\n", value);
        }

        // Ignore the other event types for now (buttons and init)
    }

    close_joystick(fd);
    return 0;

}
